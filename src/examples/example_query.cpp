#include <iostream>
#include <exception>
#include <string>
#include <interface.hpp>  // trigno::network::Client

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: query_tca <address>"<< std::endl;
        return 1;
    }

    // construct query text
    std::string query_text = "";
    for (int idx = 2; idx < argc; idx++) {
        query_text += std::string(argv[idx]) + " ";
    }

    // initialize client
    auto address = std::string(std::string(argv[1]));

    trigno::network::Interface client(address);

    std::cout << client.version() << std::endl;
    std::cout << "Please input query/command...\n\n" << std::endl;


    try {
        auto response = client.query(query_text);
        std::cout << query_text << " >> " << response << std::endl;
    } catch (std::runtime_error&) {
        std::cout << "Unable to perform query!" << std::endl;
    }


    if (argc == 3) {
        std::string cmd = "";
        while (cmd != "exit") {
            // get query
            std::cin >> cmd;
            // parse query & catch errors
            try {
                auto response = client.query(cmd);
                std::cout << "\n[" << cmd << "] >> " << response << std::endl;
                std::cout <<"\r---------------\n";
            } catch (std::runtime_error&) {
                std::cout << "ERROR: Unable to perform query!" << std::endl;
            }
        }
    }

    return 0;
}
