#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>           // std::ifstream
#include <type_traits>       // std::is_same<>()
#include "std/indexer.hpp"   // std::index<>
#include "std/utils.hpp"     // std::tokenize
#include "sensor.hpp"        // trigno::Sensor, trigno::sensor::find
#include "sample.hpp"        // trigno::Sample
#include "frame.hpp"

namespace trigno {

Frame::Frame(size_t n_sensors, size_t n_channels) :
    std::index< Sample >(n_sensors, Sample(sensor::ID::_1, n_channels)) {
        /* ... */
        _data.reserve(sensor::ID::MAX);
}



Frame::Frame(const Frame& frame, const sensor::List& sensor_ids) {
    // if no ids are specified, call copy constructor/assignment
    if (sensor_ids.size() == 0) {
        *this = frame;
        return;
    }
    // add each sensor in the list
    for (int s = 0; s < frame.size(); s++) {
        if (sensor::find(frame[s].id(), sensor_ids) == true) {
            emplace_back(frame.key(s), frame[s]);
        }
    }
}



Frame::Frame(const Frame& frame, const sensor::Labels& sensor_labels) {
    // if no ids are specified, call copy constructor/assignment
    if (sensor_labels.size() == 0) {
        *this = frame;
        return;
    }
    // add each sensor in the list
    for (const auto& label : sensor_labels) {
        try {
            emplace_back(label, frame[label]);
        } catch (std::out_of_range&) {
            continue;
        }
    }
}



size_t Frame::dim() const noexcept {
    size_t dim = 0;
    for (const auto& sensor : *this) {
        dim += sensor.size();
    }
    return dim;
}



void Frame::reset(const std::vector< Sample::Value >& values) {
    if (!values.size()) {
        throw std::invalid_argument("[...]");
    }
    // for multiple values
    if (values.size() >= dim()) {
        size_t idx = 0;
        for (auto& sensor : (*this)) {
            for (auto& channel : sensor) {
                channel = values[idx];
                if (values.size() > 1) {
                    idx++;
                }
            }
        }
    }
}



void Frame::reset(Sample::Value value) {
    reset({ value });
}



int Frame::find(sensor::ID id) const {
    // linear search - underlying container is not sorted!
    for (int idx = 0; idx < size(); idx++) {
        if ((*this)[idx].id() == id) {
            return idx;
        }
    }
    return -1;
}



Frame Frame::operator[](const sensor::List& sensor_ids) const {
    return Frame(*this, sensor_ids);
}



Frame Frame::operator[](const sensor::Labels& sensor_labels) const {
    return Frame(*this, sensor_labels);
}



template < /* Frame template specialization */ >
std::string Frame::header< Frame >(const Frame& frame, const std::string& delimiter) {
    std::string header;

    // get labels in bulk
    auto labels = frame.get_keys< std::vector< std::string > >();
    size_t idx = 1;

    for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
        for (int channel_idx = 0; channel_idx < frame[sensor_idx].size(); channel_idx++) {
            if (!frame.key(sensor_idx).empty()) {
                header += labels[sensor_idx];
            } else {
                header += ("S#" + std::to_string(sensor_idx + 1));
            }
            header += ("C#" + std::to_string(channel_idx));
            if (channel_idx < frame[sensor_idx].size() - 1) {
                header += delimiter;
            }
        }
    }

    return header;
}



template < /* Frame::Stamped template specialization */ >
std::string Frame::header< Frame::Stamped >(const Frame::Stamped& frame, const std::string& delimiter) {
    std::string header;
    // add time label if input time is stamped
    header += "Time";
    header += delimiter;

    // get labels in bulk
    auto labels = frame.value().get_keys< std::vector< std::string > >();
    size_t idx = 1;

    for (int sensor_idx = 0; sensor_idx < frame.value().size(); sensor_idx++) {
        for (int channel_idx = 0; channel_idx < frame.value()[sensor_idx].size(); channel_idx++) {
            if (!frame.value().key(sensor_idx).empty()) {
                header += labels[sensor_idx];
            } else {
                header += ("S#" + std::to_string(sensor_idx + 1));
            }
            header += ("C#" + std::to_string(channel_idx));
            if (channel_idx < frame.value()[sensor_idx].size() - 1) {
                header += delimiter;
            }
        }
    }

    return header;
}



template < /* Frame template specialization */ >
std::string Frame::to_string< Frame >(const Frame& frame, const std::string& delimiter) {
    std::string data_str;

    for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
        for (int channel_idx = 0; channel_idx < frame[sensor_idx].size(); channel_idx++) {
            data_str += std::to_string(frame[sensor_idx][channel_idx]);
            if (channel_idx < frame[sensor_idx].size() - 1) {
                data_str += delimiter;
            }
        }
    }

    return data_str;
}



template < /* Frame::Stamped template specialization */ >
std::string Frame::to_string< Frame::Stamped >(const Frame::Stamped& frame, const std::string& delimiter) {
    std::string data_str;

    // add time label
    data_str += std::to_string(frame.key);
    data_str += delimiter;

    for (int sensor_idx = 0; sensor_idx < frame.value().size(); sensor_idx++) {
        for (int channel_idx = 0; channel_idx < frame.value()[sensor_idx].size(); channel_idx++) {
            data_str += std::to_string(frame.value()[sensor_idx][channel_idx]);
            if (channel_idx < frame.value()[sensor_idx].size() - 1) {
                data_str += delimiter;
            }
        }
    }

    return data_str;
}



Frame Frame::EMG(const network::MultiSensorConfiguration& configuration) {
    /* ... */
    Frame out;
    out._data.reserve(sensor::ID::MAX);
    // add sensors samples for active sensors
    for (int s = 0; s < configuration.size(); s++) {
        if (configuration[s].isActive()) {
            // create empty sensor sample in place
            out._data.emplace_back(configuration[s].id(), configuration[s].nEMGChannels());
            // assign sensor label to frame sample
            out._data.back().key = configuration.key(s);
        }
    }

    return out;
}



Frame Frame::AUX(const network::MultiSensorConfiguration& configuration) {
    /* ... */
    Frame out;
    out._data.reserve(sensor::ID::MAX);
    // add sensors samples for active sensors
    for (int s = 0; s < configuration.size(); s++) {
        if (configuration[s].isActive()) {
            // create empty sensor sample in place
            out._data.emplace_back(configuration[s].id(), configuration[s].nAUXChannels());
            // assign sensor label to frame sample
            out._data.back().key = configuration.key(s);
        }
    }

    return out;
}



template < typename T >
T Frame::initialize(const std::string& header, const std::string& delimiter) {
    // initialize empty frame & reference to accomodate different output types
    Frame frame;

    // parse header string
    auto labels = std::tokenize(header, delimiter);
    size_t sensor_id = 1;
    size_t n_channels = 0;
    for (const auto& label : labels) {
        auto words = std::tokenize(label, "C#", true /* single split */);
        if (words.size() <= 1) {
            // assumes single channel, appends frame w/ label and resets counter
            frame.emplace_back(label, static_cast< sensor::ID >(sensor_id++), n_channels + 1);
            n_channels = 0;
        } else {
            // cast to numeric and compare to max
            auto ch = atoi(words[1].data());
            if (ch > n_channels) {
                n_channels = ch;
            }
        }
    }
    if (n_channels) {
        frame.emplace_back(labels.back(), static_cast< sensor::ID >(sensor_id++), n_channels + 1);
    }

    // assign timestamp on stamped types
    if (std::is_same< T, Frame::Stamped >()) {
        return Stamped(TimeStamp(0.0) /* no ts value to parse on header! */, std::move(frame));
    }
    return frame;
}

// @note no need to explicitely specialize initialize(), same definition can fit different types
template Frame Frame::initialize< Frame >(const std::string&, const std::string&);
template Frame::Stamped Frame::initialize< Frame::Stamped >(const std::string&, const std::string&);



template < /* Frame template specialization */ >
void Frame::load< Frame >(Frame& frame, const std::string& data, const std::string& delimiter) {
    // split input & header strings according to delimiter
    auto str_values = std::tokenize(data, delimiter);

    // assert number of input values
    if (str_values.size() != frame.dim()) {
        throw std::runtime_error("Not enough data values in input string!");
    }

    // cast str to float
    std::vector< Sample::Value > values;
    values.reserve(str_values.size());
    for (const auto& val : str_values) {
        values.emplace_back(std::stof(val));
    }

    // assign values to frame
    frame.reset(values);
}



template < /* Frame::Stamped template specialization */ >
void Frame::load< Frame::Stamped >(Frame::Stamped& frame, const std::string& data, const std::string& delimiter) {
    // split input & header strings according to delimiter
    auto str_values = std::tokenize(data, delimiter);

    // assert number of input values
    if (str_values.size() != (frame.value().dim() + 1)) {
        throw std::runtime_error("Not enough data values in input string!");
    }

    // cast str to float
    std::vector< Sample::Value > values;
    values.reserve(str_values.size());
    for (const auto& val : str_values) {
        values.emplace_back(std::stof(val));
    }

    // assign values to frame
    // @note   exception is thrown if dimensions do not match frame
    TimeStamp ts = std::stof(str_values[0]);
    frame.key = ts;
    frame.value().reset(values);
}



template < typename T >
T Frame::load(const std::string& path, size_t index, const std::string& header, const std::string& delimiter) {
    // get header from file
    std::ifstream file(path);

    // load first frame as header if required
    std::string labels = header;
    if (header.empty()) {
        std::getline(file, labels);
    }

    // skip to desired index (a.k.a. line)
    std::string line;
    size_t l = 0;
    while (l != index) {
        std::getline(file, line);
        l++;

        if (!file.good()) {
            throw std::runtime_error("No data @ index[" + std::to_string(index) + "]");
        }
    }

    // initialize output frame from header (empty)
    T out = initialize< T >(labels, delimiter);
    // load frame content into empty frame
    load< T >(out, line, delimiter);
    return out;
}

// @note no need to explicitely specialize load() overload, same definition can fit different types
template Frame Frame::load< Frame >(const std::string&, size_t, const std::string&, const std::string&);
template Frame::Stamped Frame::load< Frame::Stamped >(const std::string&, size_t, const std::string&, const std::string&);

}  // namespace trigno



std::ostream& operator<<(std::ostream& ostream, const trigno::Frame& frame) {
    ostream << trigno::Frame::to_string(frame);
    return ostream;
}



std::ostream& operator<<(std::ostream& ostream, const trigno::Frame::Stamped& frame) {
    ostream << frame.key << "," << trigno::Frame::to_string(frame);
    return ostream;
}



std::istream& operator>>(std::istream& istream, trigno::Frame& frame) {
    std::string data;
    istream >> data;
    trigno::Frame::load< trigno::Frame >(frame, data);
    return istream;
}



std::istream& operator>>(std::istream& istream, trigno::Frame::Stamped& frame) {
    std::string data;
    istream >> data;
    trigno::Frame::load< trigno::Frame::Stamped >(frame, data);
    return istream;
}

