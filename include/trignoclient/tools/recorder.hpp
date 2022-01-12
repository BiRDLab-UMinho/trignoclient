#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_RECORDER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_RECORDER_HPP_

#include <string>
#include <atomic>
#include "std/basic_executor.hpp"          // std::basic_timed_executor<>
#include "core/sequence.hpp"               // trigno::Sequence
#include "network/basic_data_client.hpp"   // trigno::network::BasicDataClient


namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Basic data/session recorder, that constructs a data sequence from incoming data frames for a limited span.
///
/// @note       Useful to get data signals pertaining to complete trials/experiments or to implement detailed acquisition protocols.
///
class Recorder : public std::basic_timed_executor< const sensor::List& > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  data_client  Data client instance.
    /// @param      out          Sequence to write to. Recorder has no ownership over output data container, which allows appending frames to previously recorded data.
    ///
    explicit Recorder(trigno::network::BasicDataClient* data_client, trigno::Sequence* out);

    //--------------------------------------------------------------------------
    /// @brief      Record data values for *sensors* synchronously for *time* ms.
    ///
    /// @param[in]  time     Duration to record.
    /// @param[in]  sensors  Sensors to record. Defaults to full set.
    ///
    /// @note       Redirects to base class implementation, override only provided to define default parameter values (cleaner interface).
    ///
    void run(const Duration& time, const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Record data values for *sensors* synchronously, for indefinite time (until input data is no longer available).
    ///
    /// @param[in]  sensors  Sensors to record. Defaults to full set.
    ///
    /// @note       Redirects to base class implementation, provided for convenience.
    ///
    void run(const sensor::List& sensors = sensor::all);

    //--------------------------------------------------------------------------
    /// @brief      Record data values for *sensors* asynchronously for *time* ms.
    ///
    /// @param[in]  time     Duration to record.
    /// @param[in]  sensors  Sensors to record. Defaults to full set.
    ///
    /// @note       Redirects to base class implementation, override only provided to define default parameter values (cleaner interface).
    ///
    void launch(const Duration& time, const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Record data values for *sensors* asynchronously, for indefinite time (until input data is no longer available).
    ///
    /// @param[in]  sensors  Sensors to record. Defaults to full set.
    ///
    /// @note       Redirects to base class implementation, provided for convenience.
    ///
    void launch(const sensor::List& sensors = sensor::all);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Reads & exports a single *DataFrame* from the data client.
    ///             Called on a loop by base class run/launch members.
    ///
    void execute(const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Network client (data source).
    ///
    /// @note       Single client can be shared among different recorders, but can't be used simultaneously.
    ///
    trigno::network::BasicDataClient* _data_client;

    //--------------------------------------------------------------------------
    /// @brief      Output sequence (where to copy read frames to).
    ///
    trigno::Sequence* _out;

    //--------------------------------------------------------------------------
    /// @brief      Mutex to claim thread ownership during execution.
    ///
    std::mutex _mx;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_RECORDER_HPP_
