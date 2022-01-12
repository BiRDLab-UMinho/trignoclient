#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICEMGPROTOCOL_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICEMGPROTOCOL_HPP_

#include <string>
#include <vector>
#include <initializer_list>
#include "core/duration.hpp"     // trigno::Duration
#include "core/sequence.hpp"     // trigno::Sequence
#include "network/client.hpp"    // trigno::network::Client


namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Base abstract class that describes an EMG acquisition protocol.
///
/// @note       Designed as an utility to customize Trigno SDK applications.
///             Users should derive from BasicProtocol, and provide implementations for prepare(), run() and finalize().
///             Protocols are started by calling start().
///
/// @note       Generic utility functions to save, plot and wait experiment are provided as member functions, and can be called by
///             custom protocol types as needed.
///
/// @note       Higher-level protocol manager that acts on top of an existing *Client*. Sensor & system configuration is done through the *Client* instance
///             and out of the scope of any protocol class.
///
class BasicProtocol {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  client   Network client in order to interface with a Trigno server.
    ///
    explicit BasicProtocol(const std::string& name, network::Client* client);

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~BasicProtocol();

    //--------------------------------------------------------------------------
    /// @brief      Get name of the protocol.
    ///
    const std::string& name() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Run experimental protocol.
    ///
    /// @param[in]  repetitions  Number of repetitions.
    ///
    void run(size_t repetitions = 1);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Name of the protocol
    ///
    std::string _name;

    //--------------------------------------------------------------------------
    /// @brief      Client instance.
    ///
    trigno::network::Client* _client;

    //--------------------------------------------------------------------------
    /// @brief      Preliminary routine, to prepare experimental protocol.
    ///
    /// @note       Useful to implement steps required @ start of experiment, e.g. display instructions, calibrate, MVC acquisition, etc.
    ///
    virtual void prepare();

    //--------------------------------------------------------------------------
    /// @brief      Run experimental protocol.
    ///
    /// @param[in]  duration_s  Duration of the execution.
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///
    /// @throw      std::runtime_error in case of network error/communication with server fails.
    ///
    virtual void execute() = 0;

    //--------------------------------------------------------------------------
    /// @brief      Post-run routine, to finish experimental protocol.
    ///
    /// @note       Useful to implement steps required @ end of experiment, e.g. data post-processing, file export, etc.
    ///
    virtual void finalize();

    //--------------------------------------------------------------------------
    /// @brief      Display message tool.
    ///
    /// @param[in]  text  Text to print to console.
    ///
    static void message(const std::string& text);

    //--------------------------------------------------------------------------
    /// @brief      Pause/block protocol execution for limited time, with optionally loop message printing.
    ///
    /// @param[in]  duration        Duration (in seconds).
    /// @param[in]  message_format  Message format. Default to empty text (no output).
    /// @param[in]  log_interval    Time to wait between text output. Ignored if *message* is empty, defaults to 1s.
    ///
    static void wait(const Duration& time, const std::string& message = "", const Duration& log_interval = Duration(1000));

    //--------------------------------------------------------------------------
    /// @brief      Records (& optionally exports) data for given *sensors*, adding it to the sequence buffer (or writing to local file).
    ///
    /// @param      data_client  Data client to use.
    /// @param[in]  duration     Duration of the data acquisition. Defaults to 0.0 i.e. continuous until user stops streaming.
    /// @param[in]  sensors      List of active/relevant sensors. Remaining active sensors are ignored! Defaults to empty list i.e. all sensors are used.
    /// @param[in]  log_interval Time delay betwen
    ///
    /// @return     Sequence of data frames received during record operation.
    ///
    static trigno::Sequence record(trigno::network::BasicDataClient* data_client, const sensor::List& sensors = sensor::all,
                                                                                  const Duration& time = Duration(0),
                                                                                  const Duration& log_interval = Duration(1000));

    //--------------------------------------------------------------------------
    /// @brief      Exports up to *n_frames* from data sequence buffer to file.
    ///
    /// @param[in]  data         Sequence to export.
    /// @param[in]  path         Path to target file. Defaults to empty string i.e. data is not exported.
    /// @param[in]  export_step  Step of the export operation (number of frames to write @ step).
    /// @param[in]  sensors      List of active/relevant sensors. Remaining active sensors are ignored! Defaults to empty list i.e. all sensors are used.
    /// @param[in]  n_frames     Number of frames to export. Defaults to 1000 i.e. all available frames are exported.
    ///
    /// @note       By design, *data* is partially consumed by export operation (*n_frames* are removed).
    ///
    static void save(trigno::Sequence&   data,
                     const std::string&  path,
                     const sensor::List& sensors  = sensor::all,
                     size_t              n_frames = 1000,
                     bool                discard  = true);

    //--------------------------------------------------------------------------
    /// @brief      Promps the user for text input.
    ///
    /// @param[in]  query    User query/message.
    /// @param[in]  timeout  Timeout value for return (wo/ input). Defaults to 0.0 i.e. infinite timeout.
    ///
    /// @return     User input text.
    ///
    static std::string prompt(const std::string& query, const Duration& timeout = Duration(0));

#ifdef WITH_PLOTTING
    //--------------------------------------------------------------------------
    /// @brief      Plots a trajectory.
    ///
    /// @param[in]  data   Input data to plot.
    /// @param[in]  label  Data label (Y axis).
    /// @param[in]  title  Plot title.
    ///
    static void plot(const trigno::Sequence& data, const std::string& label, const std::string& title);
#endif  // WITH_PLOT
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICEMGPROTOCOL_HPP_
