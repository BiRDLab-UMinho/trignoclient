#include <string>
#include <vector>
#include <initializer_list>
#include <iostream>
#include "duration.hpp"       // trigno::Duration
#include "client.hpp"         // trigno::network::Client
#include "recorder.hpp"       // trigno::network::Recorder
#include "logger.hpp"         // trigno::tools::Logger
#include "exporter.hpp"       // trigno::tools::Exporter
#include "configuration.hpp"  // trigno::Sensor
#include "frame.hpp"          // trigno::Frame
#include "sequence.hpp"       // trigno::Sequence
#include "basic_protocol.hpp"

namespace trigno::tools {

BasicProtocol::BasicProtocol(const std::string& name, network::Client* client) :
    _name(name),
    _client(client) {
        /* ... */
}



BasicProtocol::~BasicProtocol() {
    /* ... */
}



const std::string& BasicProtocol::name() const noexcept {
    return _name;
}



void BasicProtocol::run(size_t repetitions) {
    /* ... */
    prepare();
    for (int r = 0; r < repetitions; r++) {
        execute();  // custom protocols must override execute
    }
    finalize();
}



void BasicProtocol::prepare() {
    /* ... */
    message("----------------------------------------");
    message("\tRunning protocol \"" + _name + "\"");
    message("----------------------------------------");
    /* ... */
}



void BasicProtocol::finalize() {
    /* ... */
    message("----------------------------------------");
    message("\tFinished protocol \"" + _name + "\"");
    message("----------------------------------------");
    /* ... */
}



void BasicProtocol::message(const std::string& text) {
    std::cout << "\e[1m" /* bold on */ << ">> " << text  << "\e[0m" /* bold off */ << std::endl;
}



void BasicProtocol::wait(const Duration& time, const std::string& message, const Duration& log_interval) {
    // instantiate & configure waiter object (handles asynchronous timers)
    Logger reporter(log_interval, message);
    // run reporter thread synchronously (block until completed)
    reporter.run(time);
}



// trigno::Sequence BasicProtocol::record(trigno::network::BasicDataClient* data_client, const Duration& time, const SensorList& sensors, const Duration& log_interval, Exporter* exporter = nullptr);
trigno::Sequence BasicProtocol::record(trigno::network::BasicDataClient* data_client, const sensor::List& sensors, const Duration& time, const Duration& log_interval) {
    trigno::Sequence out;
    // instantiate recorder for given client
    // @note       trigno::tools::Recorder handles exporting!
    Recorder recorder(data_client, &out);
    Logger   reporter(log_interval, "Recording data (%1.1fs remaining)");

    // launch executors
    recorder.launch(time);
    reporter.launch(time);

    // wait (dummy, incase recorder exists prematurely)
    std::wait_for({ &reporter, &recorder });

    return out;
}



void BasicProtocol::save(trigno::Sequence& data, const std::string& path, const sensor::List& sensors, size_t n_frames, bool discard) {
    // instantiate exporter
    // auto file = Exporter::file(path, sensors);
    // Exporter exporter(&file, &data);

    // block until export is completed!
    // exporter.run(data);
}



std::string BasicProtocol::prompt(const std::string& query, const Duration& timeout) {
    // print query/message
    std::cout << query << std::endl;
    // get user response (block until user presses 'Enter' a.k.a. '\n')
    std::string ans;
    std::cin >> ans;

    return ans;
}



#ifdef WITH_PLOTTING
void BasicProtocol::plot(const trigno::Sequence& data, const std::string& label, const std::string& title) {
    // // create figure
    // plt::figure_size(1200, 780);

    // // get time vector (a.k.a. indexer keys cf. std::indexer<>)
    // auto times = data.get_keys< std::vector< float > >();

    // // for each sensor in sequence, get data vector & plot
    // for (int s = 0; s < data.front().size(); s++) {
    //     values = data.get(s);
    //     plt::named_plot(labels[col_idx], times, col_vec);
    // }

    // // configure plot
    // plt::title(title);
    // plt::ylabel(label);
    // plt::xlabel("Time");
    // plt::legend();

    // // prompt plot (blocking)
    // plt::show();
}
#endif  // WITH_PLOT

}  // namespace trigno::tools
