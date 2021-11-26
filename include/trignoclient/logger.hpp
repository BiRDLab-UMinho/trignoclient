#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_LOGGER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_LOGGER_HPP_

#include <string>
#include <mutex>
#include "std/basic_executor.hpp"    // std::basic_timed_executor
#include "duration.hpp"              // trigno::Duration

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Simple tool to synchronously/asynchronously print messages to stdout.
///
class Logger : public std::basic_timed_executor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  message_delay   Delay between message output. Defaults to 1000 (1s).
    /// @param[in]  message_format  Message format on which to print e.g. "%f seconds remaining!"
    ///
    explicit Logger(const Duration& message_delay = Duration(1000), const std::string& message_format = "");

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Reads & saves a single *DataFrame* from the data client
    ///
    void execute() override;

    //--------------------------------------------------------------------------
    /// @brief      Delay between message output (if any).
    ///
    const Duration _message_delay;

    //--------------------------------------------------------------------------
    /// @brief      Formatted message to output during wait.
    ///
    std::string _message_format;

    //--------------------------------------------------------------------------
    /// @brief      Mutex to claim thread ownership during execution.
    ///
    /// @note       Required to print to stdout, as stream is shared among different threads!
    ///
    std::mutex _mx;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_LOGGER_HPP_
