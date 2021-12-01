#include <string>
#include "duration.hpp"         // trigno::Duration
#include "interface.hpp"        // trigno::network::Interface
#include "configuration.hpp"    // trigno::network::MultiSensorConfiguration, ...
#include "emg_data_client.hpp"  // trigno::network::EMGDataClient
#include "aux_data_client.hpp"  // trigno::network::AuxDataClient
#include "client.hpp"

namespace trigno::network {

Client::Client() :
    system(&server),
    base(&server),
    sensors(&server),
    connection(&server),
    EMG(&sensors),
    AUX(&sensors) {
        /* ... */
}



Client::Client(const std::string& address, size_t command_port, size_t emg_data_port, size_t aux_data_port, const Duration& timeout) :
    system(&server),
    base(&server),
    sensors(&server),
    connection(&server),
    EMG(&sensors),
    AUX(&sensors) {
        /* ... */
        initialize(address, command_port, emg_data_port, aux_data_port, timeout);
}



Client::~Client() {
    /* ... */
    shutdown();
}



bool Client::connected() const {
    return (server.connected() && EMG.connected() && AUX.connected());
}



void Client::initialize(const std::string& address, size_t command_port, size_t emg_data_port, size_t aux_data_port, const Duration& timeout) {
    // establish TCP/IP connections
    server.connect(address, command_port, timeout);
    EMG.connect(address, emg_data_port, timeout);
    AUX.connect(address, aux_data_port, timeout);

    // update system configuration (trigger)
    system.get();

    // update base information (serial & firmware)
    base.get();

    // update sensor configuration (...)
    sensors.get();

    // update network/connection parameters ...()
    connection.get();

    // set connection as master!
    if (!server.command("MASTER", "NEW MASTER")) {
        throw std::runtime_error("Unable to set connection as master!");
    }
}



void Client::shutdown() {
    // establish TCP/IP connections
    server.disconnect();
    EMG.disconnect();
    AUX.disconnect();
}

}  // namespace trigno::network
