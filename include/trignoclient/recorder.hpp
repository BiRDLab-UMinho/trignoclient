#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_RECORDER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_RECORDER_HPP_

#include <string>
#include <atomic>
#include "std/basic_executor.hpp"  // std::basic_timed_executor
#include "basic_data_client.hpp"   // trigno::network::BasicDataClient
#include "sequence.hpp"            // trigno::Sequence

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Basic data/session recorder, that constructs a data sequence from incoming data frames for a limited span.
///
/// @note       Useful to get data signals pertaining to complete trials/experiments or to implement detailed acquisition protocols.
///
class Recorder : public std::basic_timed_executor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  data_client  Data client instance.
    /// @param      out          Sequence to write to. Recorder has no ownership over output data container, which allows appending frames to previously recorded data.
    ///
    explicit Recorder(trigno::network::BasicDataClient* data_client, trigno::Sequence* out);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Reads & exports a single *DataFrame* from the data client.
    ///
    void execute() override;

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
