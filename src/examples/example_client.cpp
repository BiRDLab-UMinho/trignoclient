#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include "trignoclient.hpp"  // trigno::network::Client

using namespace trigno;
using namespace std::literals::chrono_literals;

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
        printf("Usage: ./example_client <SERVER_ADDRESS>\n");
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

    // add local label to sensor #1 (idx -> 0)
    client.sensor.label(sensor::ID::_1) = "TRAPEZIUS";

    // connected, parse system configuration & attempt read
    try {
        // look for paired sensors!
        if (!client.sensor.getActive().size()) {
            std::cout << "No paired/active sensors, aborting..." << std::endl;
            return 1;
        }

        // send start trigger & schedule stop to 5s from now
        // implementation is compatible with chrono literals, which improves readability and cleaner syntax.
        client.system.start(5s);
        std::cout << "Starting..." << std::endl;

        // wait for system to start streaming
        // trigno server has a significant delay between 'start' command and streaming begins
        if (!client.EMG.waitForData()) {
            std::cout << "No data is being streamed, make sure system is properly configured/connected!" << std::endl;
            return 2;
        }
        // Frame emg_data = client.EMG.read(sensor::all, 1s);   // alternative

        // listen to incoming data frames
        // loop will keep running until:
        //      1. streaming stops (according to configured time);
        //      2. connection is lost (network is unreachable);
        //      3. data read timeout is exceeded (user closes TCU);
        while (1) {
            // frame initialization on return of read()
            Frame::Stamped emg_data = client.EMG.read(sensor::all, 1s);
            // use shift operator for cleaner syntax
            // not as versatile as it does not allow configurable time out (set ConnectionConfiguration::IO_TIMEOUT for global config)
            // client.EMG >> emg_data;

            std::cout << emg_data.key << std::endl;
            // trigno::write(std::cout, emg_data) << std::endl;

            // print reading for the first channel of the labelled sensor
            // std::cout << <<  " TRAPEZIUS -> " << emg_data["TRAPEZIUS"] << client.sensor["TRAPEZIUS"].units()[0] << "\n";
        }
    } catch (std::runtime_error& error) {
        std::cout << "Error:" << error.what() << std::endl;
        return 3;
    }

    return 0;
}
