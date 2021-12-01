#include <mutex>
#include "std/basic_executor.hpp"   // std::basic_timed_executor<>
#include "basic_data_client.hpp"    // trigno::network::BasicDataClient
#include "sequence.hpp"             // trigno::DataSequence
#include "recorder.hpp"

namespace trigno::tools {

Recorder::Recorder(trigno::network::BasicDataClient* data_client, trigno::Sequence* out) :
    std::basic_timed_executor(),
    _data_client(data_client),
    _out(out) {
        /* ... */
}



void Recorder::run(Duration time, const sensor::List& sensors) {
    _sensors = sensors;
    run(time);
}



void Recorder::launch(Duration time, const sensor::List& sensors) {
    _sensors = sensors;
    launch(time);
}



void Recorder::execute() {
    std::unique_lock< std::mutex > lock(_mx);
    // get frame from data client
    if (_sensors.size()) {
        (*_out) << _data_client->read(_sensors);
    } else {
        (*_out) << _data_client->read(/* default -> all sensors */);
    }
}

}  // namespace trigno::tools
