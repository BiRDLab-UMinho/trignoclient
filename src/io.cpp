#include <type_traits>
#include <iostream>
#include <string>
#include <fstream>
#include "std/utils.hpp" // std::tokenize, std::print_into, std::from_string
#include "sample.hpp"    // trigno::Sample
#include "frame.hpp"     // trigno::Frame, trigno::Frame::Stamped
#include "sequence.hpp"  // trigno::Sequence
#include "io.hpp"


namespace trigno {

template < typename T, typename >
std::ostream& write(std::ostream& ostream, const T& data, char delimiter) {
    return std::print_into(ostream, data, delimiter);
}

template std::ostream& write< trigno::Sample >(std::ostream&, const Sample&, char);
template std::ostream& write< trigno::Frame >(std::ostream&, const Frame&, char);



template < /* Frame::Stamped specialization */ >
std::ostream& write< Frame::Stamped, void >(std::ostream& ostream, const Frame::Stamped& data, char delimiter) {
    ostream << data.key << delimiter;
    return write(ostream, data.get(), delimiter);
}



template < /* Sequence::Range specialization */ >
std::ostream& write< Sequence::Range, void >(std::ostream& ostream, const Sequence::Range& data, char delimiter) {
    // for sequence objects, a new line is used for each frame, bypassing DEFAULT_DELIMITER
    for (auto& frame : data) {
        write(ostream, frame, delimiter) << '\n';
    }
    return ostream;
}


//------------------------------------------------------------------------------


template < typename T, typename >
std::istream& read(std::istream& istream, T& data, char delimiter) {
    return std::load_from(istream, data, delimiter);
}

template std::istream& read< trigno::Sample >(std::istream&, Sample&, char);
template std::istream& read< trigno::Frame >(std::istream&, Frame&, char);



template < /* Frame::Stamped specialization */ >
std::istream& read< Frame::Stamped, void >(std::istream& istream, Frame::Stamped& data, char delimiter) {
    istream >> data.key;
    istream.ignore(std::numeric_limits< std::streamsize >::max(), delimiter);
    return read(istream, data.get(), delimiter);
}



template < /* Sequence::Range specialization */ >
std::istream& read< Sequence::Range, void >(std::istream& istream, Sequence::Range& data, char delimiter) {
    // for sequence objects, a new line is used for each frame, bypassing DEFAULT_DELIMITER
    for (auto& frame : data) {
        read(istream, frame, delimiter);
        istream.ignore(std::numeric_limits< std::streamsize >::max(), '\n');  // ignore rest of line
    }
    return istream;
}


//------------------------------------------------------------------------------


template < /* Sample specialization */ >
std::string header< Sample >(const Sample& data, char delimiter, const std::string& extra) {
    std::string header;

    for (int channel_idx = 0; channel_idx < data.size(); channel_idx++) {
        // add ID & label as "[ID]:Label[0]"
        header += std::to_string(data.id());
        if (!extra.empty()) {
            header += ":";
            header += extra;
        }
        if (data.size() > 1) {
            header += ("[" + std::to_string(channel_idx) + "]");
        }
        if (channel_idx < data.size() - 1) {
            header += delimiter;
        }
    }

    return header;
}



template < /* Frame specialization */ >
std::string header< Frame >(const Frame& data, char delimiter, const std::string& extra) {
    std::string header;

    // call overloadeed header() for each sample
    for (int idx = 0; idx < data.size(); idx++) {
        header += trigno::header(data[idx], delimiter, data.key(idx));
        if (idx < data.size() - 1) {
            header += delimiter;
        }
    }

    return header;
}



template < /* Frame::Stamped specialization */ >
std::string header< Frame::Stamped >(const Frame::Stamped& data, char delimiter, const std::string& extra) {
    std::string header;

    // add time label if input time is stamped
    header += "Time";
    header += delimiter;
    header += trigno::header(data.get(), delimiter);

    return header;
}



template < /* Sequence::Range specialization */ >
std::string header< Sequence::Range >(const Sequence::Range& data, char delimiter, const std::string& extra) {
    return header(data[0]);
}


//------------------------------------------------------------------------------


template < /* Sample specialization */ >
Sample initialize< Sample >(const std::string& header, char delimiter) {
    // tokenize, check size
    auto labels = std::tokenize(header, delimiter);

    sensor::ID id = sensor::ID::_1;

    for (const auto& label : labels) {
        size_t sc_pos = label.find_first_of(':');
        if (sc_pos != std::string::npos) {
            // parse ID -> attempt to cast to sensor::ID
            id = static_cast< sensor::ID >(std::stoi(label.substr(0, sc_pos + 1)));
            break;
        }
    }

    return Sample(id, labels.size());
}



template < /* Frame specialization */ >
Frame initialize< Frame >(const std::string& header, char delimiter) {
    // instantiate empty output frame
    Frame out;

    // parse header string
    auto labels = std::tokenize(header, delimiter);
    std::vector< size_t > channels(sensor::all.size(), 0);
    std::vector< std::string > names(sensor::all.size(), "");

    // @note       labels -> 5:Trapezius[1] (id #5, name 'Trapezius', channel #2)
    for (const auto& label : labels) {
        //
        size_t sc_pos = label.find_first_of(':');
        if (sc_pos != std::string::npos) {
            // parse ID -> attempt to cast to sensor::ID
            auto id = static_cast< sensor::ID >(std::stoi(label.substr(0, sc_pos + 1)));
            // increment counter
            if (!channels[id]) {
                channels[id] = 1;
            }
            // eval if single or multi channel
            size_t br_pos = label.find_last_of('[');
            if (br_pos != std::string::npos) {
                // parse channel index -> update if > than current channel_idx
                auto idx = std::stoi(label.substr(br_pos, 1));
                // check if max channel
                if (idx + 1 > channels[id]) {
                    channels[id] = idx + 1;
                }
            }
            // parse name with remiander of label
            // only first label for each id is assigned, subsequent are ignored
            // no check is perfomed on label content
            if (names[id].empty()) {
                names[id] = label.substr(sc_pos, br_pos != std::string::npos ? (br_pos - sc_pos) : br_pos /* npos */);
            }

        } else {
            continue;
        }
    }

    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]) {
            out.emplace_back(names[i], static_cast< sensor::ID >(i), channels[i]);
        }
    }

    return out;
}



template < /* Frame::Stamped specialization */ >
Frame::Stamped initialize< Frame::Stamped >(const std::string& header, char delimiter) {
    // split header string
    auto half = std::tokenize(header, delimiter, true);

    // ignore if time stamp label
    // if (half.find("Time") != std::string::npos || half.find("time") != std::string::npos || half != "t") {
    //         throw std::invalid_argument("[" + __func__ + "] Invalid header string!");
    // }

    return Frame::Stamped(0.0, initialize< Frame >(half[1], delimiter));
}



template < /* Sequence specialization */ >
Sequence initialize< Sequence >(const std::string& header, char delimiter) {
    Sequence out;
    return (out << initialize< Frame::Stamped >(header, delimiter));
}


//------------------------------------------------------------------------------


template < typename T, typename >
void save(const std::string& path, const T& data, bool with_header, char delimiter) {
    // open file
    std::ofstream file(path, std::ofstream::out | std::ofstream::app);
    // append header when required
    if (with_header) {
        file << trigno::header(data, delimiter) << "\n";
    }
    // call generic std::print_into() function
    trigno::write(file, data, delimiter);

    file.close();
}

template void save< Sample >(const std::string&, const Sample&, bool, char);
template void save< Frame >(const std::string&, const Frame&, bool, char);
template void save< Frame::Stamped >(const std::string&, const Frame::Stamped&, bool, char);
template void save< Sequence::Range >(const std::string&, const Sequence::Range&, bool, char);


//------------------------------------------------------------------------------


template < typename T, typename >
void load(const std::string& path, T& data, bool initialize, size_t line, char delimiter) {
    // open file
    std::ifstream file(path, std::ofstream::out | std::ofstream::app);
    // load header when required
    // calls initialize to properly dimension data (replacing contents!)
    // assumes header is @ first line of file
    if (initialize) {
        std::string header;
        std::getline(file, header);
        data = trigno::initialize< T >(header, delimiter);
    }

    // skip input stream content until matching line
    for (size_t l = 1; l < line; l++) {
        std::skipline(file);
    }

    // call generic std::print_into() function
    trigno::read(file, data, delimiter);

    file.close();
}

template void load< Sample >(const std::string&, Sample&, bool, size_t, char);
template void load< Frame >(const std::string&, Frame&, bool, size_t, char);
template void load< Frame::Stamped >(const std::string&, Frame::Stamped&, bool, size_t, char);
template void load< Sequence >(const std::string&, Sequence&, bool, size_t, char);

}  // namespace trigno



template < typename T, typename >
std::ostream& operator<<(std::ostream& ostream, const T& data) {
    return write(ostream, data, DEFAULT_DELIMITER);
}



template < typename T, typename >
std::istream& operator>>(std::istream& istream, T& data) {
    return read(istream, data, DEFAULT_DELIMITER);
}
