#include <string>
#include <thread>
#include <iostream>
#include "std/tcp_client.hpp"         // std::tcp_client
#include "core/duration.hpp"          // trigno::Duration
#include "network/configuration.hpp"  // trigno::network::ConnectionConfiguration
#include "network/interface.hpp"

namespace trigno::network {

const std::string Interface::termination_word = "\r\n";

Interface::Interface(const std::string& address, size_t port, const Duration& timeout) {
    /* ... */
    connect(address, port, timeout);
}


Interface::~Interface() {
    /* ... */
    disconnect();
}



bool Interface::connected() const {
    return _network.is_connected();
}



const std::string& Interface::version() const {
    return _protocol_version;
}



void Interface::connect(const std::string& address, size_t port, const Duration& timeout) {
    _network.connect(address, port, timeout);
    _protocol_version = std::string(_network.read_until('\n').data());
}



void Interface::disconnect() {
    _network.disconnect();
}



void Interface::query(const std::string& query, std::string* response, const Duration& timeout) {
    // add stop char to query
    // @note: stop char is protocol-specific
    std::string stop = "";
    char CR = 13;
    char LF = 10;
    stop.append(1, CR);
    stop.append(1, LF);
    stop.append(1, CR);
    stop.append(1, LF);

    // write command to socket
    _network.write(query + stop, timeout);

    if (response != nullptr) {
        auto buffer = _network.read_until('\n', timeout);
        if (buffer.size() > 4 && buffer[buffer.size() - 4] && buffer.back() == '\n') {
            *response = std::string(buffer.data(), buffer.size() - 4 /* discard stop chars */);
            // std::cout << "last char: " << buffer[buffer.size() - 5] << std::endl;
        }
    }
}



std::string Interface::query(const std::string& query, const Duration& timeout) {
    std::string response;
    this->query(query, &response, timeout);
    return response;
}



bool Interface::command(const std::string& command, const std::string& success_response, const Duration& timeout) {
    std::string response;
    try {
        if (success_response.empty()) {
            query(command, nullptr, timeout);
            return true;
        } else {
            query(command, &response, timeout);
            if (response != success_response) {
                return false;
            }
            return true;
        }
    } catch (boost::system::system_error&) {
        return false;
    }

    return false;
}



bool Interface::waitFor(const std::string& target, const Duration& timeout, size_t max_attempts) {
    for (int rep = 0; rep < max_attempts; rep++) {
        auto buffer   = _network.read_until('\n', timeout);
        auto response = std::string(buffer.data());
        // search for target string in TCA response
        if (response.find(target) != std::string::npos) {
            return true;
        }
    }

    return false;
}



std::future< std::string > Interface::schedule(const Duration& delay, const std::string& query, const Duration& timeout) {
    std::future< std::string > response =  std::async(std::launch::async, [this, delay, query, timeout ] {
        std::this_thread::sleep_for(delay);
        return this->query(query, timeout);
    });
    return response;
}


}  // namespace trigno::network
