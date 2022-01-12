#include <initializer_list>
#include <cassert>
#include "network/client.hpp"
#include "tools/basic_protocol.hpp"
#include "tools/mvc.hpp"

namespace trigno::tools {

MVC::MVC(network::Client* client, const std::initializer_list< uint >& sensors, float contract_time, float relax_time) :
    BasicProtocol(client, sensors),
    _contract_time(contract_time),
    _relax_time(relax_time),
    _envelope(&_sequence) {
        /* ... */
        assert(repetitions > 0);
}



MVC::~MVC() {
    /* ... */
}



void MVC::execute() {
    do {
        // instruct and block execution during relax duration
        message("Please contract muscle after: ");
        log(_relax_time, "\r%1.1f seconds....");

        // record/gather EMG samples for pre-set contract time
        _sequence << record(_client->EMG, _contract_time);

        // instruct and block execution during relax duration
        message("Relax muscle  for: ");
        log(_relax_time, "\r%1.1f seconds....");

        // filter results (RMS window filter + Peak detection)
        _envelope.run();
        _peak.run();

        // plot (blocking)
        plot({ _sequence, _envelope.out() });

        // get user confirmation
        auto ans = prompt("Confirm? Y/N", 5.0);
        bool valid = (ans == "Y" || ans == "y") ? true : false;

    } while (!valid);
}



void MVC::finalize() {
    message("Exporting data...");
    // save whole trajectory as csv file
    save(_sequence, "mvc_raw.csv");
    save(_envelope.out(), "mvc_filtered.csv");

    BasicProtocol::finalize();
}

}  // namespace trigno::tools
