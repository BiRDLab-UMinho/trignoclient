#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include "std/not_implemented.hpp"       // std:not_implemented
#include "basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "configuration.hpp"             // trigno::network::MultiSensorConfiguration
#include "sequence.hpp"                  // trigno::Sequence::Range
#include "io.hpp"
#include "exporter.hpp"

namespace trigno::tools {

Exporter::Exporter(const std::string& path, Sequence* data, char delimiter) :
    BasicSequenceProcessor(),
    _path(_path),
    _data(data),
    _delimiter(delimiter) {
        /* ... */
}



void Exporter::target(const std::string& path) noexcept {
    _path = path;
}



void Exporter::source(Sequence* data) noexcept {
    _data = data;
}



void Exporter::execute() {
    // export range to file, delegating to trigno::save implementaiton
    trigno::save(_path, _range, false, _delimiter);
    // remove/consume frames if valid sequence header
    if (_data != nullptr) {
        // lock mutex to access shared resources
        std::unique_lock< std::mutex > lock(_mutex);
        // call bulk erase() function
        _data->erase(_range.begin(), _range.end());
    }
}

}  // namespace trigno::tools
