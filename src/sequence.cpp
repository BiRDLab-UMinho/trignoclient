#include <utility>            // std::move()
#include <cmath>              // sqrt(), pow()
#include <string>
#include <fstream>
#include "io.hpp"
#include "sensor.hpp"         // trigno::Sensor
#include "frame.hpp"          // trigno::Frame
#include "sequence.hpp"

namespace trigno {

void Sequence::discard(size_t n_samples) {
    for (int i = 0; i < n_samples; ++i) {
        _data.pop_front();
    }
}



void Sequence::add(const Frame::Stamped& frame) {
    // if (size() > 0) {
    //     if (back().sensors() != frame.sensors()) {
    //         throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (mismatched sensors)!");
    //     }
    // }
    if (frame.key <= _data.back().key) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (< time stamp)!");
    }
    _data.emplace_back(frame);
}



void Sequence::add(const Frame::TimeStamp& time, const Frame& frame) {
    // if (size() > 0) {
    //     if (back().sensors() != frame.sensors()) {
    //         throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (mismatched sensors)!");
    //     }
    // }
    if (time <= _data.back().key) {
        throw std::invalid_argument("[" + std::string(__func__) + "] Invalid frame (< time stamp)!");
    }
    _data.emplace_back(time, frame);
}



Sequence& Sequence::operator<<(Frame::Stamped& frame) {
    emplace_back(std::move(frame));
    return (*this);
}



Sequence& Sequence::operator<<(Frame::Stamped&& frame) {
    emplace_back(frame);
    return (*this);
}



Sequence& Sequence::operator<<(Sequence& sequence) {
    // @note it may be better to use insert(_data.end(), sequence.begin(), sequence.end())
    for (auto& frame : sequence) {
        (*this) << frame;
    }
    return (*this);
}



Sequence& Sequence::operator<<(Sequence&& sequence) {
    for (auto& frame : sequence) {
        (*this) << frame;
    }
    return (*this);
}



Sequence::Range Sequence::range(float time, float window, float sample_rate, float overlap) {
    // convert time intervals to number of samples
    unsigned int window_size  = sqrt(pow(window * sample_rate, 2));
    unsigned int overlap_size = sqrt(pow(overlap * sample_rate, 2));
    // construct range iterator on return!
    return Sequence::Range(this, find(time), window_size, overlap_size);
}


Sequence::ConstRange Sequence::range(float time, float window, float sample_rate, float overlap) const {
    // convert time intervals to number of samples
    unsigned int window_size  = sqrt(pow(window * sample_rate, 2));
    unsigned int overlap_size = sqrt(pow(overlap * sample_rate, 2));
    // construct range iterator on return!
    return Sequence::ConstRange(this, find(time), window_size, overlap_size);
}



Sequence::operator Range() {
    return range(0, size());
}



Sequence::operator ConstRange() const {
    return range(0, size());
}



Sequence::Signal Sequence::extract(const Sequence::Range& range, sensor::ID id, size_t channel) {
    Signal out;
    // out.reserve(range.size());  // std::series wraps around std::deque, no reserve()

    for (const Frame::Stamped& frame : range) {
        // @note searches linearly within frames for sample with matching id
        //       this accounts for cases where no all frames in sequence have sensor data
        auto idx = frame.get().find(id);
        if (idx < 0) {
            continue;
        }
        out.emplace_back(frame.key, frame.get()[channel]);
    }

    return out;
}


Sequence::Signal Sequence::extract(const Sequence::Range& range, const sensor::Label& label, size_t channel) {
    Signal out;
    // out.reserve(range.size());  // std::series wraps around std::deque, no reserve()

    for (const Frame::Stamped& frame : range) {
        auto idx = frame.get().find(label);
        if (idx < 0) {
            continue;
        }
        out.emplace_back(frame.key, frame.get()[channel]);
    }

    return out;
}

}  // namespace trigno

//------------------------------------------------------------------------------
