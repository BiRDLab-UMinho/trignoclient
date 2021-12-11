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

    // create empty frame sequence to hold read values
    Sequence data;

    // send start trigger & schedule stop to 5s from now
    // implementation is compatible with chrono literals, which improves readability and cleaner syntax.
    client.system.start(5s);
    std::cout << "Starting..." << std::endl;

    // dummy read
    std::cout << "Waiting for server to start streaming...." << std::endl;
    if (!client.EMG.waitForData()) {
        std::cout << "No data is being streamed, make sure system is properly configured/connected!" << std::endl;
        return 2;
    }

    // instantiate recorder object (for EMG data) & console logger
    tools::Recorder recorder(&client.EMG, &data);
    tools::Logger logger(1s, "Recording for %.2f s\n");
    tools::Exporter exporter("exported.csv", &data);

    // start recording/logging
    logger.launch(10s);
    recorder.launch(10s);
    exporter.launch(data.begin(100));

    // wait for executors
    // @note    alternatively, one can run one executor synchronously with others in parallel.
    recorder.wait();
    logger.wait();
    exporter.wait();

    std::cout << "Read " << data.size() << " frames." << std::endl;

    // export sequence to file
    trigno::save("data.csv", data);

    return 0;
}
