#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include <thread>
#include "trignoclient.hpp"

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
    std::cout << "Loaded " << data.size() << " values!" << std::endl;
    std::cout << descriptor(data.front< Frame::Stamped >()) << " [" << data.front().size() << " sensors]" << std::endl;
    std::cout << "First frame: " << data.front< Frame::Stamped >() << std::endl;
    std::cout << "1001st frame: " << data.at< Frame::Stamped >(1000) << std::endl;

    // export
    // tools::Iterative< tools::Exporter > writer("other.csv", &data);
    // writer.launch(data.begin(1000));
    // writer.wait();
}
