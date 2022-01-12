#include <cmath>
#include <limits>
#include "core/frame.hpp"                       // trigno::Frame
#include "core/sequence.hpp"                    // trigno::Sequence::Range
#include "tools/basic_sequence_processor.hpp"   // trigno::tools::BasicSequenceProcessor
#include "tools/sequence_metrics.hpp"

namespace trigno::tools {

void Maximum::execute(Sequence::Range&& range, const sensor::List& sensors) {
    _value = Frame(range[0]);
    _value.set(-1.0 * std::numeric_limits< Sample::Value >::max());

    for (const Frame& frame : range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                if (frame[sensor_idx][channel_idx] > _value[sensor_idx][channel_idx]) {
                    _value[sensor_idx][channel_idx] = frame[sensor_idx][channel_idx];
                }
            }
        }
    }
}


//------------------------------------------------------------------------------


void Minimum::execute(Sequence::Range&& range, const sensor::List& sensors) {
    _value = Frame(range[0]);
    _value.set(std::numeric_limits< Sample::Value >::max());

    for (const Frame& frame : range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                if (frame[sensor_idx][channel_idx] < _value[sensor_idx][channel_idx]) {
                    _value[sensor_idx][channel_idx] = frame[sensor_idx][channel_idx];
                }
            }
        }
    }
}


//------------------------------------------------------------------------------


void Average::execute(Sequence::Range&& range, const sensor::List& sensors) {
    _value = Frame(range[0]);
    _value.set(0.0);

    for (const Frame& frame : range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                _value[sensor_idx][channel_idx] += frame[sensor_idx][channel_idx];
            }
        }
    }

    for (auto& sensor : _value) {
        for (auto& channel : sensor) {
            channel /= range.size();
        }
    }
}


//------------------------------------------------------------------------------


void RMS::execute(Sequence::Range&& range, const sensor::List& sensors) {
    _value = Frame(range[0]);
    _value.set(0.0);

    for (const Frame& frame : range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                _value[sensor_idx][channel_idx] += pow(frame[sensor_idx][channel_idx], 2);
            }
        }
    }

    for (auto& sensor : _value) {
        for (auto& channel : sensor) {
            channel = sqrt(channel /range.size());
        }
    }
}



}  // namespace trigno::tools
