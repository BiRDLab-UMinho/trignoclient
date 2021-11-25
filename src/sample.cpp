#include <vector>
#include <iostream>
#include <cstring>     // std::memcpy
#include "sample.hpp"  // trigno::sample::ID

namespace trigno {

Sample::Sample(sensor::ID id, size_t n_channels, const char* raw_data) :
    _id(id),
    _data(n_channels) {
        /* ... */
        if (raw_data != nullptr) {
            for (int ch = 0; ch < n_channels; ch++) {
                std::memcpy(&_data[ch], raw_data, sizeof(_data[ch]));
            }
        }
}



Sample::Sample(sensor::ID id, const std::vector< float >& values) :
    _id(id),
    _data(values) {
        /* ... */
}



sensor::ID Sample::id() const noexcept {
    return _id;
}



size_t Sample::size() const noexcept {
    return _data.size();
}



bool Sample::empty() const noexcept {
    return _data.empty();
}



float& Sample::operator[](size_t channel) {
    if (channel >= _data.size()) {
        throw std::out_of_range("[" + std::string(__func__) + "] Invalid channel index!!");
    }
    return _data[channel];
}



const float& Sample::operator[](size_t channel) const {
    if (channel >= _data.size()) {
        throw std::out_of_range("[" + std::string(__func__) + "] Invalid channel index!!");
    }
    return _data[channel];
}



Sample::operator const Sample::Value&() const noexcept {
    return _data.front();
}



Sample::Value Sample::average() const {
    auto avg = 0.0;
    for (const auto& value : _data) {
        avg += value;
    }
    return (avg/=size());
}



std::vector< float >::iterator Sample::begin() {
    return _data.begin();
}



std::vector< float >::const_iterator Sample::begin() const {
    return _data.begin();
}



std::vector< float >::iterator Sample::end() {
    return _data.end();
}



std::vector< float >::const_iterator Sample::end() const {
    return _data.end();
}

}  // namespace trigno



std::ostream& operator<<(std::ostream& ostream, const trigno::Sample& sample) {
    std::string delimiter = ", ";
    for (int ch = 0; ch < sample.size(); ch++) {
        ostream << sample[ch];
        if (ch < sample.size() - 1) {
            ostream << delimiter;
        }
    }
    return ostream;
}
