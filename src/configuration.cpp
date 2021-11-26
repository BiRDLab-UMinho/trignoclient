#include <string>
#include <vector>
#include <exception>
#include <utility>
#include <cassert>
#include <future>                   // std::async
#include "std/not_implemented.hpp"  // std::not_implemented
#include "std/indexer.hpp"          // std::index<>
#include "interface.hpp"            // trigno::network::Interface
#include "basic_configurator.hpp"   // trigno::network::BasicConfigurator
#include "configuration.hpp"

namespace trigno::network {

// constexpr size_t SensorConfiguration::NSENSORS = 16;

SensorConfiguration::SensorConfiguration(sensor::ID id, Interface* network) :
    BasicConfigurator(network),
    _id(id),
    _paired(false) {
        /* .. */
        assert(_id <= sensor::ID::MAX);
        reset();
}




void SensorConfiguration::reset() {
    // _paired = false;
    _active = false;
    _type = static_cast< char >(0);
    _mode = 0;
    _n_channels = 0;
    _n_emg_channels = 0;
    _n_aux_channels = 0;
    _start_index = 0;
    _firmware = "UNKNOWN";
    _serial   = "UNKNOWN";
    _low_range = true;
    _narrow_bandwidth = true;
}



bool SensorConfiguration::get() {
    /* .. */
    // initialize response container
    std::string response;

    try {
        // pair state
        _network->query("SENSOR " + std::to_string(_id) + " PAIRED?", &response);
        if (!response.empty()) {
            _paired = (response == "YES") ? true : false;
        }
    } catch (std::exception&) {
        // ...
    }

    // if not paired, further queries will fail, reset & abort!
    if (!_paired) {
        reset();
        return false;
    }

    try {
        // active state
        // @note       checking active before paired a
        _network->query("SENSOR " + std::to_string(_id) + " ACTIVE?", &response);
        if (!response.empty()) {
            _active = (response == "YES") ? true : false;
        }
    } catch (std::exception&) {
        // ...
    }

    // if not active, reset & abort (keep paired status to true), as garbage information will be returned on further queries
    if (!_active) {
        reset();
        return false;
    }

    try {
        // sensor type
        _network->query("SENSOR " + std::to_string(_id) + " TYPE?", &response);
        if (!response.empty()) {
            _type = response.front();
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // sensor operating mode
        _network->query("SENSOR " + std::to_string(_id) + " MODE?", &response);
        if (!response.empty()) {
            _mode = atoi(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // total number of channels (mode-dependent)
        _network->query("SENSOR " + std::to_string(_id) + " CHANNELCOUNT?", &response);
        if (!response.empty()) {
            _n_channels = atoi(response.data());
            _n_samples_frame.resize(_n_channels);
            _sample_rate.resize(_n_channels);
            _gain.resize(_n_channels);
            _units.resize(_n_channels);
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // total number of EMG channels (mode-dependent)
        _network->query("SENSOR " + std::to_string(_id) + " EMGCHANNELCOUNT?", &response);
        if (!response.empty()) {
            _n_emg_channels = atoi(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // total number of AUX channels (mode-dependent)
        _network->query("SENSOR " + std::to_string(_id) + " AUXCHANNELCOUNT?", &response);
        if (!response.empty()) {
            _n_aux_channels = atoi(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // start index of sensor data in data frame
        response.clear();
        _network->query("SENSOR " + std::to_string(_id) + " STARTINDEX?", &response);
        if (!response.empty()) {
            _start_index = atoi(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // firmware version
        _network->query("SENSOR " + std::to_string(_id) + " FIRMWARE?", &response);
        if (!response.empty() && response.find("INVALID") != std::string::npos) {
            _firmware = response;
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // serial number
        _network->query("SENSOR " + std::to_string(_id) + " SERIAL?", &response);
        if (!response.empty() && response.find("INVALID") != std::string::npos) {
            _serial = response;
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // samples p/frame for each channel (mode-dependent)`
        // @note sensor/channel number must be >0, otherwise TCA will crash!
        for (int ch = 0; ch < _n_channels; ch++) {
            _network->query("SENSOR " + std::to_string(_id) + " CHANNEL " + std::to_string(ch + 1) + " SAMPLES?", &response);
            if (!response.empty()) {
                _n_samples_frame[ch] = atoi(response.data());
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // samples p/frame for each channel (mode-dependent)
        // @note sensor/channel number must be >0, otherwise TCA will crash!
        for (int ch = 0; ch < _n_channels; ch++) {
            _network->query("SENSOR " + std::to_string(_id) + " CHANNEL " + std::to_string(ch + 1) + " RATE?", &response);
            if (!response.empty()) {
                _sample_rate[ch] = atof(response.data());
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // gain for each channel (mode-dependent)
        // @note sensor/channel number must be >0, otherwise TCA will crash!
        for (int ch = 0; ch < _n_channels; ch++) {
            _network->query("SENSOR " + std::to_string(_id) + " CHANNEL " + std::to_string(ch + 1) + " GAIN?", &response);
            if (!response.empty()) {
                _gain[ch] = atof(response.data());
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // gain for each channel (mode-dependent)
        // @note sensor/channel number must be >0, otherwise TCA will crash!
        for (int ch = 0; ch < _n_channels; ch++) {
            _network->query("SENSOR " + std::to_string(_id) + " CHANNEL " + std::to_string(ch + 1) + " UNITS?", &response);
            if (!response.empty()) {
                _units[ch] = response;
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // sensor type
        _network->query("SENSOR " + std::to_string(_id) + " RANGE?", &response);
        if (response == "LOW") {
            _low_range = true;
        } else if (response == "HIGH") {
            _low_range = false;
        }
        // else no valid response was received, *do not change _low_range*
    } catch (std::exception&) {
        // ...
    }

    try {
        // sensor type
        _network->query("SENSOR " + std::to_string(_id) + " BANDWIDTH?", &response);
        if (response == "NARROW") {
            _narrow_bandwidth = true;
        } else if (response == "WIDE") {
            _narrow_bandwidth = false;
        }
        // else no valid response was received, *do not change _narrow_bandwidth*
    } catch (std::exception&) {
        // ...
    }

    // ...
    return true;
}



bool SensorConfiguration::set() {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
    return true;
}



SensorConfiguration::operator std::string() const {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
}



sensor::ID SensorConfiguration::id() const noexcept {
    return _id;
}



const std::string& SensorConfiguration::serial() const noexcept {
    return _serial;
}



const std::string& SensorConfiguration::firmware() const noexcept {
    return _firmware;
}



char SensorConfiguration::type() const noexcept {
    return _type;
}



size_t SensorConfiguration::mode() const noexcept {
    return _mode;
}



size_t SensorConfiguration::nChannels() const noexcept {
    return _n_channels;
}



size_t SensorConfiguration::nEMGChannels() const noexcept {
    return _n_emg_channels;
}



size_t SensorConfiguration::nAUXChannels() const noexcept {
    return _n_aux_channels;
}



size_t SensorConfiguration::startIndex() const noexcept {
    return _start_index;
}



const std::vector< size_t >& SensorConfiguration::samplesPerFrame() const noexcept {
    return _n_samples_frame;
}



const std::vector< float >& SensorConfiguration::sampleRate() const noexcept {
    return _sample_rate;
}



const std::vector< float >& SensorConfiguration::gain() const noexcept {
    return _gain;
}



const std::vector< std::string >& SensorConfiguration::units() const noexcept {
    return _units;
}



bool SensorConfiguration::lowRange() const noexcept {
    return _low_range;
}



bool SensorConfiguration::narrowBandwidth() const noexcept {
    return _narrow_bandwidth;
}



bool SensorConfiguration::isPaired() const noexcept {
    return _paired;
}



bool SensorConfiguration::isActive() const noexcept {
    return _active;
}



bool SensorConfiguration::pair() {
    /* .. */
    std::string response;
    _network->query("SENSOR " + std::to_string(_id) + " PAIR", &response);
    // block until any message with 'COMPLETE' is received
    if (_network->waitFor("COMPLETE")) {
        return true;
    }
    return false;
}



bool SensorConfiguration::setMode(size_t mode) {
    /* .. */
    std::string response;
    _network->query("SENSOR " + std::to_string(_id) + " SETMODE" + std::to_string(mode), &response);
    // parse response and check success state
    if (response.find("OK") != std::string::npos) {
        _mode = mode;
        return true;
    }
    return false;
}



bool SensorConfiguration::setRange(bool low) {
    /* .. */
    std::string response;
    _network->query("SENSOR " + std::to_string(_id) + " SETRANGE" + (low ? "LOW" : "HIGH"), &response);
    // parse response and check success state
    if (response.find("OK") != std::string::npos) {
        _low_range = low;
        return true;
    }
    return false;
}



bool SensorConfiguration::setBandwidth(bool narrow) {
    /* .. */
    std::string response;
    _network->query("SENSOR " + std::to_string(_id) + " SETBANDWITH" + (narrow ? "NARROW" : "WIDE"), &response);
    // parse response and check success state
    if (response.find("OK") != std::string::npos) {
        _narrow_bandwidth;
        return true;
    }
    return false;
}


//------------------------------------------------------------------------------


BaseInformation::BaseInformation(Interface* network) :
    BasicConfigurator(network) {
        /* .. */
        reset();
}



void BaseInformation::reset() {
    _firmware = "UNKNOWN";
    _serial   = "UNKNOWN";
}



bool BaseInformation::get() {
    /* .. */
    std::string response;

    try {
        // firmware version
        _network->query("BASE FIRMWARE?", &response);
        if (!response.empty()) {
            _firmware = response;
            _firmware.pop_back();  // @note base information queries reply with an addition '\n' @ end!
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // serial number -> @note base information queries reply with an addition '\n' @ end!
        _network->query("BASE SERIAL?", &response);
        if (!response.empty()) {
            _serial = response;
            _serial.pop_back();  // @note base information queries reply with an addition '\n' @ end!
        }
    } catch (std::exception&) {
        // ...
    }

    return true;
}



bool BaseInformation::set() {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
    return true;
}



BaseInformation::operator std::string() const {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
}



const std::string& BaseInformation::serial() const noexcept {
    return _serial;
}



const std::string& BaseInformation::firmware() const noexcept {
    return _firmware;
}


//------------------------------------------------------------------------------


MultiSensorConfiguration::MultiSensorConfiguration(Interface* network) :
    // @note base class std::index can't be empty initialized due to being locked
    std::index< SensorConfiguration, std::string, true >(sensor::ID::MAX + 1, SensorConfiguration(sensor::ID::_1, network)) {
        // @note       sensor indexing begins at '1'
        for (size_t idx = sensor::ID::_1; idx <= sensor::ID::MAX; idx++) {
            _data[idx].value()._id = static_cast< sensor::ID >(idx);
            key(idx) = "SENSOR #" + std::to_string(idx);
        }
}



bool MultiSensorConfiguration::get() {
    for (auto& sensor : *this) {
        sensor.get();
    }
    return true;
}



sensor::List MultiSensorConfiguration::getPaired() const {
    sensor::List paired;
    //
    for (const auto& sensor : (*this)) {
        if (sensor.isPaired()) {
            paired.emplace_back(sensor.id());
        }
    }
    return paired;
}



sensor::List MultiSensorConfiguration::getActive() const {
    sensor::List active;
    //
    for (const auto& sensor : (*this)) {
        if (sensor.isActive()) {
            active.emplace_back(sensor.id());
        }
    }
    return active;
}



std::string& MultiSensorConfiguration::label(sensor::ID id) noexcept {
    return key(id);
}



const std::string& MultiSensorConfiguration::label(sensor::ID id) const noexcept {
    return key(id);
}


//------------------------------------------------------------------------------


SystemControl::SystemControl(Interface* network) :
    BasicConfigurator(network) {
        /* .. */
        reset();
}



void SystemControl::reset() {
    _running = false;
    _trigger.start = false;
    _trigger.stop = false;
}



bool SystemControl::get() {
    /* .. */
    std::string response;

    try {
        // firmware version
        _network->query("TRIGGER?", &response);
        if (!response.empty()) {
            if (response.find("START ON") != std::string::npos) {
                _trigger.start = true;
            }
            if (response.find("STOP ON") != std::string::npos) {
                _trigger.stop = true;
            }
        }
    } catch (std::exception&) {
        // ...
    }

    return true;
}



bool SystemControl::set() {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
    return true;
}



SystemControl::operator std::string() const {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
}



bool SystemControl::isRunning() const {
    return _running;
}



const SystemControl::Trigger& SystemControl::trigger() const {
    return _trigger;
}



bool SystemControl::start(float delay) {
    /* .. */
    if (!_network->command("MASTER?", "YES")) {
        throw std::runtime_error("Connection is not master!");
    }

    if (delay > 0.0) {
        auto sent = std::async(std::launch::async, [this] { this->_network->command("START"); });
    } else {
        _network->command("START");
    }


    _running = true;

    return true;
}



bool SystemControl::stop(float delay) {
    /* .. */
    if (!_network->command("MASTER?", "YES")) {
        throw std::runtime_error("Connection is not master!");
    }

    if (delay > 0.0) {
        auto sent = std::async(std::launch::async, [this] { this->_network->command("STOP"); });
    } else {
        _network->command("STOP");
    }

    _running = false;
    return true;
}



bool SystemControl::quit() {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
    return true;
}


//------------------------------------------------------------------------------


ConnectionConfiguration::ConnectionConfiguration(Interface* network) :
    BasicConfigurator(network) {
        /* .. */
        reset();
}



void ConnectionConfiguration::reset() {
    _version = "UNKNOWN";
    _master = false;
    _backwards_compatibility = false;
    _upsampling = false;
    _big_endian = false;
    _max_emg_samples = false;
    _max_aux_samples = false;
    _frame_interval = 0.0;
}



bool ConnectionConfiguration::get() {
    /* .. */
    std::string response;

    try {
        // master/slave connection?
        _network->query("VERSION?", &response);
        if (!response.empty()) {
            _version = response;
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // master/slave connection?
        _network->query("MASTER?", &response);
        if (!response.empty()) {
            if (response == "YES") {
                _master = true;
            } else {
                _master = false;
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // backwards compatiblity (fixed sample rates)
        _network->query("BACKWARDS COMPATIBILITY?", &response);
        if (!response.empty()) {
            if (response == "YES") {
                _backwards_compatibility = true;
            } else {
                _backwards_compatibility = false;
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // upsampling (dependent on backward compatibility)
        _network->query("UPSAMPLING?", &response);
        if (!response.empty()) {
            if (response == "YES") {
                _upsampling = true;
            } else {
                _upsampling = false;
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // upsampling (dependent on backward compatibility)
        _network->query("ENDIANESS?", &response);
        if (!response.empty()) {
            if (response == "BIG") {
                _big_endian = true;
            } else {
                _big_endian = false;
            }
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // upsampling (dependent on backward compatibility)
        _network->query("MAX SAMPLES EMG?", &response);
        if (!response.empty()) {
            _max_emg_samples = atoi(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // upsampling (dependent on backward compatibility)
        _network->query("MAX SAMPLES AUX?", &response);
        if (!response.empty()) {
            _max_aux_samples = atoi(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    try {
        // upsampling (dependent on backward compatibility)
        _network->query("FRAME INTERVAL?", &response);
        if (!response.empty()) {
            _frame_interval = atof(response.data());
        }
    } catch (std::exception&) {
        // ...
    }

    return true;
}



bool ConnectionConfiguration::set() {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
    return true;
}



ConnectionConfiguration::operator std::string() const {
    /* .. */
    throw std::not_implemented(__func__);
    /* .. */
}



const std::string& ConnectionConfiguration::version() const {
    return _version;
}



bool ConnectionConfiguration::isMaster() const {
    return _master;
}



bool ConnectionConfiguration::backwardsCompatibility() const {
    return _backwards_compatibility;
}



bool ConnectionConfiguration::upsampling() const {
    return _upsampling;
}



bool ConnectionConfiguration::bigEndian() const {
    return _big_endian;
}



size_t ConnectionConfiguration::maxEMGSamples() const {
    return _max_emg_samples;
}



size_t ConnectionConfiguration::maxAUXSamples() const {
    return _max_emg_samples;
}



float ConnectionConfiguration::frameInterval() const {
    return _frame_interval;
}



bool ConnectionConfiguration::setMaster() {
    /* ... */
    try {
        std::string response;
        _network->query("MASTER", &response);
        if (!response.empty() && response.find("NEW MASTER") != std::string::npos) {
            _master = true;
        }
    } catch (std::exception&) {
        // ...
    }
    return true;
}



bool ConnectionConfiguration::setBackwardsCompability(bool backwards_compability) {
    /* ... */
    try {
        std::string response;
        _network->query("BACKWARDS COMPATIBILITY " + backwards_compability ? "ON" : "OFF", &response);
        if (!response.empty() && response.find("OK") != std::string::npos) {
            _backwards_compatibility = true;
        }
    } catch (std::exception&) {
        // ...
    }
    return true;
}



bool ConnectionConfiguration::setUpsampling(bool upsampling) {
    /* ... */
    try {
        std::string response;
        _network->query("UPSAMPLING " + upsampling ? "ON" : "OFF", &response);
        if (!response.empty() && response.find("OK") != std::string::npos) {
            _upsampling = true;
        }
    } catch (std::exception&) {
        // ...
    }
    return true;
}



bool ConnectionConfiguration::setBigEndian(bool big_endian) {
    /* ... */
    try {
        std::string response;
        _network->query("ENDIAN " + big_endian ? "BIG" : "LITTLE", &response);
        if (!response.empty() && response.find("OK") != std::string::npos) {
            _big_endian = true;
        }
    } catch (std::exception&) {
        // ...
    }
    return true;
}

}  // namespace trigno::network


//------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& ostream, const trigno::network::SensorConfiguration& sensor_configuration) {
    ostream << "ID: "               << sensor_configuration.id() << std::endl;
    ostream << "Paired|Active: "    << (sensor_configuration.isPaired() ? "Yes" : "No") << " | "
                                    << (sensor_configuration.isActive() ? "Yes" : "No") << std::endl;
    ostream << "Serial: "           << sensor_configuration.serial() << std::endl;
    ostream << "Firmware: "         << sensor_configuration.firmware() << std::endl;
    ostream << "Type: "             << sensor_configuration.type() << std::endl;
    ostream << "Mode: "             << sensor_configuration.mode() << std::endl;
    ostream << "# Channels: "       << sensor_configuration.nChannels() << std::endl;
    ostream << "# Channels (EMG): " << sensor_configuration.nEMGChannels() << std::endl;
    ostream << "# Channels (AUX): " << sensor_configuration.nAUXChannels() << std::endl;
    ostream << "Start Index: "      << sensor_configuration.startIndex() << std::endl;
    ostream << "Samples/frame: ";
    for (int ch = 0; ch < sensor_configuration.samplesPerFrame().size() /* == nChannels() */; ch++) {
        ostream << " | " << sensor_configuration.samplesPerFrame()[ch];
    }
    ostream << std::endl;
    ostream << "Sample rate: ";
    for (int ch = 0; ch < sensor_configuration.sampleRate().size() /* == nChannels() */; ch++) {
        ostream << " | " << sensor_configuration.sampleRate()[ch];
    }

    ostream << std::endl;
    ostream << "Gain: ";
    for (int ch = 0; ch < sensor_configuration.gain().size() /* == nChannels() */; ch++) {
        ostream << " | " << sensor_configuration.gain()[ch];
    }
    ostream << std::endl;
    ostream << "Units: ";
    for (int ch = 0; ch < sensor_configuration.units().size() /* == nChannels() */; ch++) {
        ostream << " | " << sensor_configuration.units()[ch];
    }
    ostream << std::endl;
    ostream << "Range: "     << (sensor_configuration.lowRange()        ? "Low (+/-5.5mV)"    : "High (+/-11mV)")  << std::endl;
    ostream << "Bandwidth: " << (sensor_configuration.narrowBandwidth() ? "Narrow (20-450Hz)" : "Wide (10-850Hz)") << std::endl;
    return ostream;
}



std::ostream& operator<<(std::ostream& ostream, const trigno::network::BaseInformation& base_information) {
    ostream << "Base Serial:"   << base_information.serial() << std::endl;
    ostream << "Base Firmware " << base_information.firmware() << std::endl;

    return ostream;
}



std::ostream& operator<<(std::ostream& ostream, const trigno::network::SystemControl& system_control) {
    ostream << "Running? "        << (system_control.isRunning() ? "Yes" : "No") << std::endl;
    ostream << "Trigger: Start: " << (system_control.trigger().start ? "Yes" : "No") <<
                     " | Stop: "  << (system_control.trigger().stop  ? "Yes" : "No") << std::endl;

    return ostream;
}



std::ostream& operator<<(std::ostream& ostream, const trigno::network::ConnectionConfiguration& connection_configuration) {
    ostream << "Version? "                 << connection_configuration.version() << std::endl;
    ostream << "Master? "                  << (connection_configuration.isMaster() ? "Yes" : "No") << std::endl;
    ostream << "Backwards Compatibility? " << (connection_configuration.backwardsCompatibility() ? "Yes" : "No") << std::endl;
    ostream << "Upsampling?"               << (connection_configuration.upsampling() ? "Yes" : "No") << std::endl;
    ostream << "Big Endian?"               << (connection_configuration.bigEndian() ? "Yes" : "No") << std::endl;
    ostream << "Max. EMG samples"          << connection_configuration.maxEMGSamples() << std::endl;
    ostream << "Max. EMG samples"          << connection_configuration.maxAUXSamples() << std::endl;
    ostream << "Frame interval: "          << connection_configuration.frameInterval() << std::endl;

    return ostream;
}
