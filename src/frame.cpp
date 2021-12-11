#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>           // std::ifstream
#include <type_traits>       // std::is_same<>()
#include "std/indexer.hpp"   // std::index<>
#include "std/utils.hpp"     // std::tokenize, std::print_into, std::load_from
#include "sensor.hpp"        // trigno::Sensor, trigno::sensor::find
#include "sample.hpp"        // trigno::Sample
#include "frame.hpp"

namespace trigno {

Frame::Frame(size_t n_sensors, size_t n_channels) :
    std::index< Sample >(n_sensors, Sample(sensor::ID::_1, n_channels)) {
        /* ... */
        _data.reserve(sensor::ID::MAX + 1);
}



Frame::Frame(const std::string& header, const std::vector< Sample::Value >& values, const std::string& delimiter) :
    std::index< Sample >() {
        _data.reserve(sensor::ID::MAX + 1);


        if (!values.empty()) {
            set(values);
        }
}



Frame::Frame(const Frame& frame, const sensor::List& sensor_ids) :
    std::index< Sample >() {
        _data.reserve(sensor_ids.size());

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



Frame::Frame(const Frame& frame, const sensor::Labels& sensor_labels) :
    std::index< Sample >() {
        _data.reserve(sensor_labels.size());

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



sensor::List Frame::sensors() const {
    sensor::List out;
    out.reserve(sensor::ID::MAX + 1);

    for (const auto& sensor : *this) {
        out.emplace_back(sensor.id());
    }

    return out;
}



size_t Frame::dim() const noexcept {
    size_t dim = 0;
    for (const auto& sensor : *this) {
        dim += sensor.size();
    }
    return dim;
}



void Frame::set(const std::vector< Sample::Value >& values) {
    if (!values.size()) {
        throw std::invalid_argument("[...]");
    }
    std::cout << "///" << std::endl;
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



void Frame::set(Sample::Value value) {
    set({ value });
}



// void Frame::assign(const std::string& data, const std::string& delimiter) {
//     // split input & header strings according to delimiter
//     auto str_values = std::tokenize(data, delimiter);

//     // assert number of input values
//     if (str_values.size() != frame.dim()) {
//         throw std::runtime_error("Not enough data values in input string!");
//     }

//     // cast str to float
//     std::vector< Sample::Value > values;
//     values.reserve(str_values.size());
//     for (const auto& val : str_values) {
//         values.emplace_back(std::stof(val));
//     }

//     // assign values to frame
//     assign(values);
// }



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

}  // namespace trigno
