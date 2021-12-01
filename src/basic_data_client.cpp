#include <string>
#include <vector>
#include <exception>
#include <type_traits>            // std::is_same<>()
#include "sensor.hpp"             // trigno::sensor
#include "configuration.hpp"      // trigno::network::MultiSensorConfiguration
#include "frame.hpp"              // trigno::Frame
#include "sequence.hpp"           // trigno::Sequence
#include "basic_data_client.hpp"

namespace trigno::network {

BasicDataClient::BasicDataClient(size_t channels, MultiSensorConfiguration* configuration) :
    _buffer(channels * sizeof(BasicDataClient::DataValue) /* == e.g. float is 4 bytes p/ channel */),
    _configuration(configuration),
    _sample_rate(0.0) {
        /* ... */
}



BasicDataClient::BasicDataClient(size_t channels, MultiSensorConfiguration* configuration, const std::string& address, size_t port, const BasicDataClient::Timeout& timeout) :
    _buffer(channels * sizeof(BasicDataClient::DataValue) /* == e.g. float is 4 bytes p/ channel */),
    _configuration(configuration),
    _sample_rate(0.0) {
        /* ... */
        connect(address, port, timeout);
}



BasicDataClient::~BasicDataClient() {
    /* ... */
    disconnect();
}



bool BasicDataClient::connected() const {
    return _network.is_connected();
}



void BasicDataClient::connect(const std::string& address, size_t port, const BasicDataClient::Timeout& timeout) {
    _network.connect(address, port, timeout);
    reset();
}



void BasicDataClient::disconnect() {
    _network.disconnect();
}



void BasicDataClient::reset() {
    _frame_idx = 0;
}



template < typename T >
T BasicDataClient::read(const sensor::List& sensors, const BasicDataClient::Timeout& timeout) {
    // update/populate buffer (read from TCP server)
    // (size of read = number of channels * 4)
    try {
        _network.read(_buffer, timeout);
    } catch (boost::exception&) {
        throw std::runtime_error("[" + std::string(__func__) + "] Unable to read frame!");
    }

    if (std::is_same< T, Frame::Stamped >()) {
        // build stamped frame on return
        // delegate to derived buildFrame() implementation -> move constructor is called when instantiating Stamped (a.k.a. std::tagged<>)
        return Frame::Stamped(/* time stamp */ (_frame_idx++) * _sample_rate,
                              /* frame data */ buildFrame(sensors));
    }
    return buildFrame(sensors);
}

template Frame BasicDataClient::read< Frame >(const sensor::List&, const BasicDataClient::Timeout&);
template Frame::Stamped BasicDataClient::read< Frame::Stamped >(const sensor::List&, const BasicDataClient::Timeout&);



template < typename T >
void BasicDataClient::read(T* out, const sensor::List& sensors, const BasicDataClient::Timeout& timeout) {
    // update/populate buffer (read from TCP server)
    // size of read -> number of channels * 4)
    try {
        _network.read(_buffer, timeout);
    } catch (boost::exception&) {
        throw std::runtime_error("[" + std::string(__func__) + "] Unable to read frame!");
    }

    // parse & assign to out if valid pointer, else skip
    if (out != nullptr) {
        // read and parse data buffer into output frame
        if (std::is_same< T, Frame::Stamped >()) {
            // delegate to derived buildFrame() implementation -> move constructor is called when instantiating Stamped (a.k.a. std::tagged<>)
            *out = Frame::Stamped(/* time stamp */ (_frame_idx++) * _sample_rate,
                                  /* frame data */ buildFrame(sensors));
        }
        *out = buildFrame(sensors);
    }
}

template void BasicDataClient::read< Frame >(Frame*, const sensor::List&, const BasicDataClient::Timeout&);
template void BasicDataClient::read< Frame::Stamped >(Frame::Stamped*, const sensor::List&, const BasicDataClient::Timeout&);



template < typename T >
void BasicDataClient::operator>>(T& out) {
    read(&out);
}

template void BasicDataClient::operator>>< Frame >(Frame& out);
template void BasicDataClient::operator>>< Frame::Stamped >(Frame::Stamped& out);



void BasicDataClient::operator>>(Sequence& sequence) {
    sequence << read();
}

}  // namespace trigno::network
