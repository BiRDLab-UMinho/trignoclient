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
        printf("Usage: ./example_io <FILE>\n");
        return 1;
    }
    auto file_path = std::string(argv[1]);

    // load sequence from file
    auto data = load< Sequence >(file_path);

    //
    tools::Exporter writer("other.csv", &data);

    writer.launch(data.begin(10));    
    writer.wait();

    return 0;
}
