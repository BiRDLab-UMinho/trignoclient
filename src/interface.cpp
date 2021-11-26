#include <string>
#include <thread>
#include <iostream>
#include "std/tcp_client.hpp" // std::tcp_client
#include "duration.hpp"       // trigno::Duration
#include "configuration.hpp"  // trigno::network::ConnectionConfiguration
#include "interface.hpp"

namespace trigno::network {

// std::string stop = "";
// char CR = 13;
// char LF = 10;
// stop.append(1, CR);
// stop.append(1, LF);
// stop.append(1, CR);
// stop.append(1, LF);
const std::string Interface::termination_word = "\r\n";



Interface::Interface(const std::string& address, size_t port, const Duration& timeout) {
    /* ... */
    connect(address, port, timeout);
    printf("IIIIIII\n");
}



const std::string& Interface::version() const {
    return _protocol_version;
}



void Interface::connect(const std::string& address, size_t port, const Duration& timeout) {
    std::cout << "2.5." << std::endl;

    try {
        _network.connect(address, port, timeout);
    } catch (boost::system::system_error& error) {  // or boost::system::system_error -> typedef this!
        std::cout << "Unable to connect to server:\n" << error.what() << std::endl;
        // throw std::runtime_error("Failed connect!");
    }
    std::cout << "3." << std::endl;

    try {
        _protocol_version = std::string(_network.read_until('\n').data());
        // std::cout << _protocol_version << std::endl;
    } catch (boost::system::system_error&) {  // or boost::system::system_error -> typedef this!
        std::cout << "Unable to parse Network Protocol Version" << std::endl;
    }
    std::cout << "4." << std::endl;

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

}  // namespace trigno::network
