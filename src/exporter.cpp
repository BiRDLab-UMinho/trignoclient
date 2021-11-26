#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include "std/not_implemented.hpp"       // std:not_implemented
#include "basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "configuration.hpp"             // trigno::network::MultiSensorConfiguration
#include "sequence.hpp"                  // trigno::Sequence::Range
#include "exporter.hpp"

namespace trigno::tools {

Exporter::Exporter(const std::string& file, Sequence* data) :
    BasicSequenceProcessor(),
    _file(_path, std::ofstream::out | std::ofstream::app),
    _data(data) {
        /* ... */
}



Exporter::Exporter(const std::string& file, const sensor::List& sensors, Sequence* data) :
    BasicSequenceProcessor(),
    _file(file),
    _data(data) {
        /* ... */
        // _file.open(_path, std::ofstream::out | std::ofstream::app);
}



Exporter::~Exporter() {
    /* ... */
    wait();
    // forcefully close file
    if (_file.is_open()) {
        _file.close();
    }
}



std::ofstream Exporter::file(const std::string& path, const sensor::List& sensors) {
    std::ofstream file(path);
    // _file.open(_path, std::ofstream::out | std::ofstream::app);
    file.open(path, std::ofstream::out);  // no 'app' flag, file is cleared

    // ...
    throw std::not_implemented(__func__);
    // ...
    return file;
}



void Exporter::execute() {
    _file.open(_path);
    for (const auto& frame : _range) {
        // lock mutex to access shared resources
        std::unique_lock< std::mutex > lock(_mutex);
        // write data frame to stream (trigno::Frame already provides << operator overload)
        _file  << frame << std::endl;
        // erase frame if valid source pointer
        // if (_data != nullptr) {
        //     _data.erase(_it);
        // }
    }
    _file.close();
}

}  // namespace trigno::tools
