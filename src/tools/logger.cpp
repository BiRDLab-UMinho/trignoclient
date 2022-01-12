#include <string>
#include <iostream>
#include <chrono>                  // std::chrono::milliseconds
#include <thread>                  // std::this_thread::sleep_for
#include "std/basic_executor.hpp"  // std::basic_timed_executor<>
#include "tools/logger.hpp"

namespace trigno::tools {

Logger::Logger(const Duration& message_delay, const std::string& message_format) :
    std::basic_timed_executor< >(),
    _message_delay(message_delay),
    _message_format(message_format) {
        /* ... */
}



void Logger::execute() {
    if (_message_format.size()) {
        std::unique_lock< std::mutex > lock(_mx);
        // print message & flush stdout
        printf(_message_format.data(), remaining().count() / 1000.0);
        fflush(stdout);
        // sleep
        std::this_thread::sleep_for(_message_delay);
    }
}

}  // namespace trigno::network
