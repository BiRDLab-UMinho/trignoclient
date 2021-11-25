#include <string>
#include <vector>
#include "configuration.hpp"    // trigno::network::MultiSensorConfiguration
#include "data_frame.hpp"       // trigno::DataFrame
#include "aux_data_client.hpp"

namespace trigno::network {

AUXDataClient::AUXDataClient(const std::string& server_address, const MultiSensorConfiguration* configuration, int aux_port, int connect_timeout) :
    BasicDataClient(server_address, configuration, emg_port, connect_timeout) {
        /* ... */
}



AUXDataClient::~AUXDataClient() {
    /* ... */
}



void AUXDataClient::read(DataFrame* out, size_t timeout_ms) {
    if (out != nullptr) {
        // ...
        if (out->empty()) {
            *out = read(timeout_ms);
            return;
        }
        // check if frame has full size? otherwise may be invalid
        // ...
        // for each sensor
        for (auto& sensor : *out) {
            // for each channel
            for (auto& channel : sensor) {
                channel = readValue(timeout_ms);
            }
        }
    }
    throw std::invalid_argument("");
}



DataFrame AUXDataClient::read(size_t timeout_ms) {
    // instantiate new data frame
    DataFrame out;
    std::cout << "______" << std::endl;
    // for each sensor
    for (int s = 0; s < _configuration->size(); s++) {
        auto& sensor = (*_configuration)[s];
        std::cout << "reading sensor #" << s << " [" << sensor.nAUXChannels() << " channel(s)]" << std::endl;
        // create SensorSample instance in place
        out.emplace_back(sensor.nAUXChannels());
        // for each channel, read channel data from client
        for (int ch = 0; ch < sensor.nAUXChannels(); ch++) {
            std::cout << "reading sensor #" << s << " channel #" << ch << std::endl;
            out.back()[ch] = readValue(timeout_ms);
        }
        // assign descriptor
        out.descriptor(s) = _configuration->descriptor(s);
        // printf("done!\n");
    }
    return out;
}

}  // namespace trigno::network
