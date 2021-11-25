#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include "client.hpp"  // trigno::network::Client

using namespace trigno;
using namespace std::literals::chrono_literals;

void interruptHandler(int signum) {
    printf("\nUser-requested shutdown\n");
    exit(0);
}


int main(int argc, char const *argv[]) {
    // register interrupt handler
    signal(SIGINT, interruptHandler);

    // parse input arguments
    if (argc < 2) {
        printf("Usage: ./example_network <SERVER_ADDRESS>\n");
        return 1;
    }
    auto server_address = std::string(argv[1]);

    network::Client client;

    // initialize client object
    std::cout << "Estabilishing connection with TCU @" << server_address.data() << std::endl;
    client.initialize(server_address);

    // add local label to sensor #1 (idx -> 0)
    client.sensors.label(sensor::ID::_1) = "TRAPEZIUS";

    // connected, get configuration
    try {
        // look for paired sensors!
        if (!client.sensors.getActive().size()) {
            std::cout << "No paired/active sensors, aborting..." << std::endl;
            return 1;
        }

        // send start trigger
        std::cout << "Starting..." << std::endl;
        client.server.command("START");

        // listen to first data frame
        // out of the loop in order to be able to pass a longer timeout - trigno server has a significant delay between 'start' command and streaming begins
        Frame emg_data = client.EMG.read(sensor::all, 5s);

        // listen to incoming data frames
        // loop will keep running until connection is lost (network is unreachable) or data read timeout is exceeded (user closes TCU)!
        while (1) {
            try {
                // DataFrame emg_data;
                client.EMG >> emg_data;

                // initialization on return
                // auto emg_data = client.EMG.read();

                // print reading for the first channel of the labelled sensor
                std::cout << "TRAPEZIUS -> " << emg_data["TRAPEZIUS"] << client.sensors["TRAPEZIUS"].units()[0] << "\n";

            } catch (std::exception& error) {
                std::cout << "ERROR: " << error.what() << std::endl;
                break;
            }
        }

        // sent stop command
        // dummy -> only reached when server is already stopped
        client.server.command("STOP");

    } catch (std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    return 0;
}
