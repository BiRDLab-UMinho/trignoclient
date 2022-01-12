#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include "trignoclient.hpp"

using namespace trigno;
using namespace std::literals::chrono_literals;


//------------------------------------------------------------------------------
/// @brief      Example class implementing a custom demonstrative EMG protocol
///
class MyTrignoProtocol : public tools::BasicProtocol {
 public:
    MyTrignoProtocol();
    ~MyTrignoProtocol();
};



void interruptHandler(int signum) {
    printf("\nUser-requested shutdown\n");
    exit(0);
}


int main(int argc, char const *argv[]) {
    // register interrupt handler
    signal(SIGINT, interruptHandler);

    // debug/verbose ouput
    printf("##################################\n%s\n##################################\n", __FILE__);

    // parse input arguments
    if (argc < 2) {
        printf("Usage: ./example_tools <SERVER_ADDRESS>\n");
        return 1;
    }
    auto server_address = std::string(argv[1]);

    // instantiate client
    network::Client client;

    // initialize client object (connect to server & get system configuration)
    std::cout << "Estabilishing connection with TCU @" << server_address.data() << "..... " << std::flush;
    try {
        client.initialize(server_address);
    } catch (std::exception&) {
        std::cout << "Unable to initialize client, check connection & ensure TCU is running!\n";
        return 1;
    }
    std::cout << "OK\n";


    // ...

    return 0;
}
