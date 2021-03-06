#include <utility>            // std::move()
#include <cmath>              // sqrt(), pow()
#include <string>
#include <fstream>
#include <vector>
#include "core/io.hpp"
#include "core/sensor.hpp"   // trigno::Sensor
#include "core/frame.hpp"    // trigno::Frame
#include "core/sequence.hpp"

namespace trigno {

void Sequence::discard(size_t n_samples) {
    for (int i = 0; i < n_samples; ++i) {
        _data.pop_front();
    }
}



void Sequence::add(const Frame::Stamped& frame, bool sequential, bool match_sensors) {
    // compare input key (time stamp) w/ back of sequence
    if (sequential && size() && frame.key <= _data.back().key) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (< time stamp)!");
    }
    // compare input sensors w/ back of sequence (if not empty)
    if (match_sensors && size() && back().sensors() != frame.get().sensors()) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (mismatched sensors)!");
    }
    // create copy in place
    _data.emplace_back(frame);
}



void Sequence::add(Frame::Stamped&& frame, bool sequential, bool match_sensors) {
    // compare input key (time stamp) w/ back of sequence
    if (sequential && size() && frame.key <= _data.back().key) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (< time stamp)!");
    }
    // compare input sensors w/ back of sequence (if not empty)
    if (match_sensors && size() && back().sensors() != frame.get().sensors()) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (mismatched sensors)!");
    }
    // create copy in place - move constructor will be called in place by std::tagged<>
    _data.emplace_back(frame);
}



void Sequence::add(const Frame::TimeStamp& time, const Frame& frame, bool sequential, bool match_sensors) {
    // compare input key (time stamp) w/ back of sequence
    if (sequential && size() && time <= _data.back().key) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (< time stamp)!");
    }
    // compare input sensors w/ back of sequence (if not empty)
    if (match_sensors && size() && back().sensors() != frame.sensors()) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (mismatched sensors)!");
    }
    // create copy in place
    _data.emplace_back(time, frame);
}



void Sequence::add(const Frame::TimeStamp& time, Frame&& frame, bool sequential, bool match_sensors) {
    // compare input key (time stamp) w/ back of sequence
    if (sequential && size() && time <= _data.back().key) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (< time stamp)!");
    }
    // compare input sensors w/ back of sequence (if not empty)
    if (match_sensors && size() && back().sensors() != frame.sensors()) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (mismatched sensors)!");
    }
    // create copy in place - move constructor will be called in place by std::tagged<>
    _data.emplace_back(time, frame);
}



Sequence& Sequence::operator<<(const Frame::Stamped& frame) {
    _data.emplace_back(frame);
    return (*this);
}



Sequence& Sequence::operator<<(Frame::Stamped&& frame) {
    _data.emplace_back(frame);
    return (*this);
}



Sequence& Sequence::operator<<(Sequence::ConstRange range) {
    // @note consider use insert(_data.end(), sequence.begin(), sequence.end())
    for (const Frame::Stamped& frame : range) {
        (*this) << frame;
    }
    return (*this);
}



Sequence& Sequence::operator<<(Sequence&& sequence) {
    for (Frame::Stamped&& frame : sequence) {
        (*this) << frame;
    }
    return (*this);
}



Sequence::Range Sequence::range(float time, float window, float sample_rate, float overlap) {
    // convert time intervals to number of samples
    unsigned int window_size  = sqrt(pow(window * sample_rate, 2));
    unsigned int overlap_size = sqrt(pow(overlap * sample_rate, 2));
    // construct range iterator on return!
    return Sequence::Range(&_data, find(time), window_size, overlap_size);
}



Sequence::ConstRange Sequence::range(float time, float window, float sample_rate, float overlap) const {
    // convert time intervals to number of samples
    unsigned int window_size  = sqrt(pow(window * sample_rate, 2));
    unsigned int overlap_size = sqrt(pow(overlap * sample_rate, 2));
    // construct range iterator on return!
    return Sequence::ConstRange(&_data, find(time), window_size, overlap_size);
}



Sequence::operator Range() {
    return range(0, size(), size() - 1);
}



Sequence::operator ConstRange() const {
    return range(0, size(), size() - 1);
}



size_t Sequence::extract(Sequence::ConstRange range, sensor::ID id, std::vector< double >* values, std::vector< double >* times, size_t channel) {
    values->clear();
    values->reserve(range.size());
    if (times != nullptr) {
        times->clear();
        times->reserve(range.size());
    }

    size_t n = 0;
    for (const Frame::Stamped& frame : range) {
        // @note searches linearly within frames for sample with matching id
        //       this accounts for cases where not all frames in sequence have the same sensor data
        auto idx = frame.get().find(id);
        if (idx < 0) {
            continue;
        }
        n++;
        values->emplace_back(frame.get()[channel]);
        if (times != nullptr) {
            times->emplace_back(frame.key);
        }
    }

    return n;
}


size_t Sequence::extract(Sequence::ConstRange range, const sensor::Label& label, std::vector< double >* values, std::vector< double >* times, size_t channel) {
    values->clear();
    values->reserve(range.size());
    if (times != nullptr) {
        times->clear();
        times->reserve(range.size());
    }

    size_t n = 0;
    for (const Frame::Stamped& frame : range) {
        auto idx = frame.get().find(label);
        if (idx < 0) {
            continue;
        }
        n++;
        values->emplace_back(frame.get()[channel]);
        if (times != nullptr) {
            times->emplace_back(frame.key);
        }
    }

    return n;
}

}  // namespace trigno

//------------------------------------------------------------------------------
