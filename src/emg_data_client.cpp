#include <string>
#include <vector>
#include "sensor.hpp"           // trigno::Sensor
#include "configuration.hpp"    // trigno::network::MultiSensorConfiguration, trigno::network::ConnectionConfiguration
#include "frame.hpp"            // trigno::Frame
#include "emg_data_client.hpp"

namespace trigno::network {

EMGDataClient::EMGDataClient(MultiSensorConfiguration* configuration) :
    BasicDataClient(ConnectionConfiguration::EMG_DATA_CHANNELS_PER_SENSOR * sensor::ID::MAX, configuration) {
        /* ... */
}



EMGDataClient::EMGDataClient(MultiSensorConfiguration* configuration, const std::string& address, size_t emg_data_port, const EMGDataClient::Timeout& timeout) :
    BasicDataClient(ConnectionConfiguration::EMG_DATA_CHANNELS_PER_SENSOR * sensor::ID::MAX, configuration, address, emg_data_port, timeout) {
        /* ... */
        // no need to connect, BasicDataClient establishes connection on constructor!
}



void EMGDataClient::connect(const std::string& address, size_t port, const EMGDataClient::Timeout& timeout) {
    // delegates to base implementation (but with different default arguments!)
    BasicDataClient::connect(address, port, timeout);
}



void EMGDataClient::reset() {
    // call base class reset() implementation
    BasicDataClient::reset();
    // Trigno systems sample all data channels on each port @ same sample rate
    // therefore only need to check the sample rate for the first EMG channel on the first active sensor
    for (const auto& sensor : * _configuration) {
        if (sensor.isActive() && sensor.nEMGChannels()) {
            _sample_rate = sensor.sampleRate().front();  // first value is assured to be EMG channel!
            return;
        }
    }
    throw std::runtime_error("[" + std::string(__func__) + "] No active EMG channels!");
}



Frame EMGDataClient::buildFrame(const sensor::List& sensors) const {
    // intialize empty _frame
    Frame out;

    // ensure sensor list has values -> empty list returns empty frame
    // assert(sensors.size());

    // parse only requested values/sensors
    // @note configuration must be up to date!
    for (const auto& sensor_id : sensors) {
        // check if active, skip if not
        if (!(*_configuration)[sensor_id].isActive()) {
            continue;
        }
        // get start index from configuration
        auto pos = (*_configuration)[sensor_id].startIndex();
        // add sample to frame
        // @note       raw data is parsed by trigno::Sample constructor
        out.emplace_back(sensor_id, (*_configuration)[sensor_id].nEMGChannels(), &_buffer[pos * sizeof(DataValue)]);
        // preserve sensor label in output frame
        out.elements().back().key = _configuration->element(sensor_id).key;
    }

    // // parse only requested values/sensors
    // // @note configuration must be up to date!
    // for (const Sensor& sensor : *_configuration) {
    //     // check if active, skip if not
    //     if (!_sensor.isActive()) {
    //         continue;
    //     }
    //     // get start index from configuration
    //     auto pos = _sensor.startIndex();
    //     // add sample to frame
    //     // @note       raw data is parsed by trigno::Sample constructor
    //     out.emplace_back(sensor.id(), sensor.nEMGChannels(), &_buffer[pos * sizeof(DataValue)]);
    //     // preserve sensor label in output frame
    //     out.elements().back().key = _configuration->element(sensor).key;
    // }

    return out;
}

}  // namespace trigno::network
