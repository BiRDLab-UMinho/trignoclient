#include <type_traits>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include "std/utils.hpp" // std::tokenize, std::print_into, std::from_string
#include "sample.hpp"    // trigno::Sample
#include "frame.hpp"     // trigno::Frame, trigno::Frame::Stamped
#include "sequence.hpp"  // trigno::Sequence
#include "io.hpp"


namespace trigno {

std::ostream& write(std::ostream& ostream, const Sample& data, char delimiter) {
    return std::print_into(ostream, data, delimiter);
}



std::ostream& write(std::ostream& ostream, const Frame& data, char delimiter) {
    return std::print_into(ostream, data, delimiter);
}



std::ostream& write(std::ostream& ostream, const Frame::Stamped& data, char delimiter) {
    ostream << data.key << delimiter;
    return write(ostream, data.get(), delimiter);
}



std::ostream& write(std::ostream& ostream, Sequence::ConstRange data, char delimiter, bool break_line) {
    if (data.size() > 0) {
        write(ostream, data[0], delimiter);
        for (size_t f = 1; f < data.size(); f++) {
            // for sequence objects, either a new line is used for each frame (bypassing delimiter)
            ostream << (break_line ? '\n' : delimiter);
            // write content into stream
            write(ostream, data[f], delimiter);
        }
    }
    return ostream;
}


//------------------------------------------------------------------------------


std::istream& read(std::istream& istream, Sample& data, char delimiter, bool ignore_break) {
    return std::load_from(istream, data, delimiter, ignore_break);
}



std::istream& read(std::istream& istream, Frame& data, char delimiter, bool ignore_break) {
    return std::load_from(istream, data, delimiter, ignore_break);
}



std::istream& read(std::istream& istream, Frame::Stamped& data, char delimiter, bool ignore_break) {
    // read(istream, data.key, delimiter, ignore_break);
    istream >> data.key;
    // std::ignore_until(istream, delimiter);
    return read(istream, data.get(), delimiter, ignore_break);
}



std::istream& read(std::istream& istream, Sequence::Range data, char delimiter, bool ignore_break) {
    for (auto& frame : data) {
        read(istream, frame, delimiter, ignore_break);
    }
    return istream;
}


//------------------------------------------------------------------------------


std::string descriptor(const Sample& data, const std::string& name, char delimiter) {
    std::string descriptor;

    for (int channel_idx = 0; channel_idx < data.size(); channel_idx++) {
        // build descriptor as "<ID>:<Label>[<Ch>]"
        descriptor += std::to_string(data.id());
        if (!name.empty()) {
            descriptor += ":";
            descriptor += name;
        }
        if (data.size() > 1) {
            descriptor += ("[" + std::to_string(channel_idx) + "]");
        }
        if (channel_idx < data.size() - 1) {
            descriptor += delimiter;
        }
    }

    return descriptor;
}



std::string descriptor(const Frame& data, char delimiter) {
    std::string descriptor;

    // call overloadeed descriptor() for each sample
    for (int idx = 0; idx < data.size(); idx++) {
        descriptor += trigno::descriptor(data[idx], data.key(idx), delimiter);
        if (idx < data.size() - 1) {
            descriptor += delimiter;
        }
    }

    return descriptor;
}



std::string descriptor(const Frame::Stamped& data, char delimiter) {
    std::string descriptor;

    // add time label if input time is stamped
    descriptor += "Time";
    descriptor += delimiter;
    descriptor += trigno::descriptor(data.get(), delimiter);

    return descriptor;
}


//------------------------------------------------------------------------------


void extract(const std::string& input, size_t* id, std::string* label, size_t* channel, size_t buffer_size = 20) {
    char name[buffer_size] = "";
    if (sscanf(input.data(), "%lu:%[^[][%lu]", id, name, channel) == 3) {
        label->assign(name);
        return;
    }
    if (sscanf(input.data(), "%lu[%lu]", id, channel) == 2) {
        label->clear();
        return;
    }
    if (sscanf(input.data(), "%lu:%[^[]", id, name) == 2) {
        label->assign(name);
        *channel = 0;
        return;
    }
    throw std::invalid_argument("[" + std::string(__func__) + "] Invalid descriptor / unknown format!");
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
    std::vector< size_t > ids(0);
    std::vector< size_t > channels(0);
    std::vector< std::string > names(0);

    size_t last_id = 0;
    size_t last_channel = 0;
    std::string name = "";

    // @note       labels -> 5:Trapezius[1] (id #5, name 'Trapezius', channel #2)
    //             sensor and channel idxs must be ordered/sequential, otherwise are ignored
    //             first label for each sensor is used for the name
    for (const auto& label : labels) {
        size_t id;
        size_t ch;

        try {
            extract(label, &id, &name, &ch);
        } catch (std::exception&) {
            continue;
        }

        if (!ids.size() && id && ch == 0) {
            ids.push_back(id);
            names.push_back(name);
            channels.push_back(1);
            continue;
        }

        if (id > ids.back() && !ch) {
            ids.push_back(id);
            names.push_back(name);
            channels.push_back(1);
        } else if (id == ids.back() && ch == channels.back()) {
            channels.back()++;
        }
    }

    assert(ids.size() == channels.size());
    for (size_t i = 0; i < ids.size(); i++) {
        out.emplace_back(names[i], static_cast< sensor::ID >(ids[i]), channels[i]);
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


//------------------------------------------------------------------------------


template < typename T, typename >
T initialize(std::istream& istream, size_t line, char delimiter) {
    // skip input stream content until matching line
    for (size_t l = 1; l < line; l++) {
        std::skipline(istream);
    }

    // load header from first line input stream
    // wraps around initialize() overload
    // assumes header is @ first line of file
    std::string header;
    std::getline(istream, header);
    return initialize< T >(header, delimiter);
}

template Sample initialize< Sample >(std::istream&, size_t, char);
template Frame initialize< Frame >(std::istream&, size_t, char);
template Frame::Stamped initialize< Frame::Stamped >(std::istream&, size_t, char);


//------------------------------------------------------------------------------


template < typename T >
void save_impl(const std::string& path, const T& data, bool with_header, char delimiter) {
    // open file
    std::ofstream file(path, std::ofstream::out | std::ofstream::app);
    // append header when required
    if (with_header) {
        file << descriptor(data, delimiter) << "\n";
    }
    // call generic write() function
    write(file, data, delimiter);

    file.close();
}


//------------------------------------------------------------------------------


void save(const std::string& path, const Sample& sample, bool with_header, char delimiter) {
    save_impl(path, sample, with_header, delimiter);
}



void save(const std::string& path, const Frame& frame, bool with_header, char delimiter) {
    save_impl(path, frame, with_header, delimiter);
}



void save(const std::string& path, const Frame::Stamped& frame, bool with_header, char delimiter) {
    save_impl(path, frame, with_header, delimiter);
}



void save(const std::string& path, Sequence::ConstRange range, bool with_header, char delimiter, bool break_line) {
    if (!range.size()) {
        return;
    }

    // open file
    std::ofstream file(path, std::ofstream::out | std::ofstream::app);

    // append header when required
    if (with_header) {
        file << descriptor(range[0], delimiter) << (break_line ? '\n' : delimiter);
    }

    // call generic write() function
    write(file, range, delimiter, break_line);

    file.close();
}


//------------------------------------------------------------------------------


template < typename T >
void load_impl(const std::string& path, T& data, size_t line, char delimiter) {
    // open file
    std::ifstream file(path);

    // skip input stream content until matching line
    for (size_t l = 1; l < line; l++) {
        std::skipline(file);
    }

    // call generic read() function
    read(file, data, delimiter);

    file.close();
}


//------------------------------------------------------------------------------


void load(const std::string& path, Sample& sample, size_t line, char delimiter) {
    load_impl(path, sample, line, delimiter);
}



void load(const std::string& path, Frame& frame, size_t line, char delimiter) {
    load_impl(path, frame, line, delimiter);
}



void load(const std::string& path, Frame::Stamped& frame, size_t line, char delimiter) {
    load_impl(path, frame, line, delimiter);
}



void load(const std::string& path, Sequence::Range range, size_t line, char delimiter) {
    if (!range.size()) {
        return;
    }

    // open file
    std::ifstream file(path);

    // call generic read() function
    read(file, range, delimiter);

    file.close();
}


//------------------------------------------------------------------------------


template < typename T >
T parse_header_impl(std::istream& file, size_t data_line, const std::string& header, size_t header_line, char delimiter) {
    // initialize output structure according to header argument OR file stream
    auto out = header.empty() ? initialize< T >(file, header_line, delimiter) :
                                initialize< T >(header, delimiter);

    // skip input stream content until matching line
    for (size_t l = 1; l < data_line; l++) {
        std::skipline(file);
    }

    return out;
}


//------------------------------------------------------------------------------


template < typename T, typename >
T load(std::istream& istream, size_t data_line, const std::string& header, size_t header_line, char delimiter) {
    // initialize outp  ut structure according to header argument OR file stream
    auto out = parse_header_impl< T >(istream, data_line, header, header_line, delimiter);

    // call generic trigno::read() function
    read(istream, out, delimiter);

    return out;
}

template Sample load< Sample >(std::istream&, size_t, const std::string&, size_t, char);
template Frame load< Frame >(std::istream&, size_t, const std::string&, size_t, char);
template Frame::Stamped load< Frame::Stamped >(std::istream&, size_t, const std::string&, size_t, char);



template < /* Sequence specialization */ >
Sequence load< Sequence >(std::istream& istream, size_t data_line, const std::string& header, size_t header_line, char delimiter) {
    // initialize output sequence & initialize first frame according to header argument OR file stream
    Sequence out;
    out << parse_header_impl< Frame::Stamped >(istream, data_line, header, header_line, delimiter);
    // fill first frame
    trigno::read(istream, out.back< Frame::Stamped >(), delimiter);

    // loop over input content (lines)
    // calls generic trigno::read() function to assign values in plase @ end of sequence
    while (istream.good()) {
        // add a copy of last frame to be populated @ next pass (line)
        out.add(out.back());
        // out.back().set(0.0); // ensures values are set to 0 before loading
        // read values from stream into back of sequence
        trigno::read(istream, out.back< Frame::Stamped >(), delimiter);
    }

    // check if input stream has error code, otherwise last frame may be corrupted/incomplete (e.g. unexpected close or I/O error)
    if (!istream.good() && !istream.eof()) {
        out.pop_back();
    }

    return out;
}


//------------------------------------------------------------------------------


template < typename T, typename >
T load(const std::string& path, size_t data_line, const std::string& header, size_t header_line, char delimiter) {
    // open file
    std::ifstream file(path);

    // call load() overload
    auto out = load< T >(file, data_line, header, header_line, delimiter);

    file.close();
    return out;
}

template Sample load< Sample >(const std::string&, size_t, const std::string&, size_t, char);
template Frame load< Frame >(const std::string&, size_t, const std::string&, size_t, char);
template Frame::Stamped load< Frame::Stamped >(const std::string&, size_t, const std::string&, size_t, char);
template Sequence load< Sequence >(const std::string&, size_t, const std::string&, size_t, char);

}  // namespace trigno


//------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& ostream, const trigno::Sample& sample) {
    return write(ostream, sample, DEFAULT_DELIMITER);
}



std::ostream& operator<<(std::ostream& ostream, const trigno::Frame& frame) {
    return write(ostream, frame, DEFAULT_DELIMITER);
}



std::ostream& operator<<(std::ostream& ostream, const trigno::Frame::Stamped& frame) {
    return write(ostream, frame, DEFAULT_DELIMITER);
}



std::ostream& operator<<(std::ostream& ostream, trigno::Sequence::ConstRange range) {
    return write(ostream, range, DEFAULT_DELIMITER);
}



std::istream& operator>>(std::istream& istream, trigno::Sample& sample) {
    return read(istream, sample, DEFAULT_DELIMITER);
}



std::istream& operator>>(std::istream& istream, trigno::Frame& frame) {
    return read(istream, frame, DEFAULT_DELIMITER);
}



std::istream& operator>>(std::istream& istream, trigno::Frame::Stamped& frame) {
    return read(istream, frame, DEFAULT_DELIMITER);
}



std::istream& operator>>(std::istream& istream, trigno::Sequence::Range range) {
    return read(istream, range, DEFAULT_DELIMITER);
}
