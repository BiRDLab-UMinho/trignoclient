#include <cmath>
#include <numeric_limits>
#include "frame.hpp"                      // trigno::Frame
#include "basic_sequence_processor.hpp"   // trigno::tools::BasicSequenceProcessor
#include "sequence_metrics.hpp"

namespace trigno::tools {

void Maximum::execute() {
    _out = Frame(_range[0]);
    _out.reset();

    for (const Frame& frame : _range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                if (frame[sensor_idx][channel_idx] > _out[sensor_idx][channel_idx]) {
                    _out[sensor_idx][channel_idx] = frame[sensor_idx][channel_idx];
                }
            }        }
    }
}


//------------------------------------------------------------------------------


void Minimum::execute() {
    _out = Frame(_range[0]);
    _out.reset(std::numeric_limits< Sample::Value >::max());

    for (const Frame& frame : _range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                if (frame[sensor_idx][channel_idx] < _out[sensor_idx][channel_idx]) {
                    _out[sensor_idx][channel_idx] = frame[sensor_idx][channel_idx];
                }
            }
        }
    }
}


//------------------------------------------------------------------------------


void Average::execute() {
    _out = Frame(_range[0]);

    for (const Frame& frame : _range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                _out[sensor_idx][channel_idx] += frame[sensor_idx][channel_idx];
            }
        }
    }

    for (auto& sensor : _out) {
        for (auto& channel : sensor) {
            channel /= _range.size();
        }
    }
}


//------------------------------------------------------------------------------


void RMS::execute() {
    _out = Frame(_range[0]);

    for (const Frame& frame : _range) {
        for (int sensor_idx = 0; sensor_idx < frame.size(); sensor_idx++) {
            for (int channel_idx = 0; channel_idx < frame.size(); channel_idx++) {
                _out[sensor_idx][channel_idx] += pow(frame[sensor_idx][channel_idx], 2);
            }
        }
    }

    for (auto& sensor : _out) {
        for (auto& channel : sensor) {
            channel = sqrt(channel /_range.size());
        }
    }
}



}  // namespace trigno::tools
