#include <mutex>
#include "std/basic_executor.hpp"         // std::basic_timed_executor<>
#include "core/sequence.hpp"              // trigno::DataSequence
#include "network/basic_data_client.hpp"  // trigno::network::BasicDataClient
#include "tools/recorder.hpp"

namespace trigno::tools {

Recorder::Recorder(trigno::network::BasicDataClient* data_client, trigno::Sequence* out) :
    std::basic_timed_executor< const sensor::List& >(),
    _data_client(data_client),
    _out(out) {
        /* ... */
}



void Recorder::run(const Duration& time, const sensor::List& sensors) {
    basic_timed_executor< const sensor::List& >::run(time, sensors);
}



void Recorder::run(const sensor::List& sensors) {
    basic_timed_executor< const sensor::List& >::run(Duration::max(), sensors);
}



void Recorder::launch(const Duration& time, const sensor::List& sensors) {
    basic_timed_executor< const sensor::List& >::launch(time, sensors);
}



void Recorder::launch(const sensor::List& sensors) {
    basic_timed_executor< const sensor::List& >::launch(Duration::max(), sensors);
}



void Recorder::execute(const sensor::List& sensors) {
    std::unique_lock< std::mutex > lock(_mx);
    // get frame from data client
    if (sensors.size()) {
        (*_out) << _data_client->read(sensors);
    } else {
        (*_out) << _data_client->read(/* default -> all sensors */);
    }
}

}  // namespace trigno::tools
