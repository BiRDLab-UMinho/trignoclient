#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include "std/exception.hpp"                   // std::not_implemented
#include "core/sequence.hpp"                   // trigno::Sequence::Range
#include "core/io.hpp"                         // trigno::save()
#include "tools/basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "tools/exporter.hpp"

namespace trigno::tools {

Exporter::Exporter(const std::string& path, Sequence* data, char delimiter) :
    BasicSequenceProcessor(),
    _path(path),
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



void Exporter::execute(Sequence::Range&& range, const sensor::List& sensors) {
    // export range to file, delegating to trigno::save implementation
    trigno::save(_path, range, false, _delimiter);
    // @todo save frames individually, which would allow filtering according to given sensor list though [] operator
    //       not profiles, but expected to add overhead to the operation
    // for (const Frame::Stamped& frame : range) {
    //     trigno::save(_path, frame.get()[sensors], false, _delimiter);
    // }
    // remove/consume frames if valid sequence header
    if (_data != nullptr) {
        // lock mutex to access shared resources
        std::unique_lock< std::mutex > lock(_mutex);
        // call bulk erase() function
        _data->erase(range.begin(), range.end());
    }
}



template < /* specialization for iterative (continuous) exporter */ >
void Iterative< Exporter >::start() {
    // disable iterator in-/decrement if source has been assigned
    // if input sequence is being modified (frames are being deleted) then range should not change
    if (_executor._data != nullptr) {
        _step = 0;
    }
}



template < /* specialization for iterative (continuous) exporter */ >
bool Iterative< Exporter >::active() const {
    if (_executor._data != nullptr) {
        return (_range != _range + 1);
    } else {
        if (!Iterative< Exporter >::incrementable(_range, _step)) {
            std::this_thread::sleep_for(_idle_time);
            return Iterative< Exporter >::incrementable(_range, _step);
        }
    }
    return true;
}


}  // namespace trigno::tools
