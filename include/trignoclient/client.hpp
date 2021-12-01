//------------------------------------------------------------------------------
/// @file       client.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of trigno::network::Client class, the main network abstraction type in *trignoclient*.
///             Users should use Client as the main interface with remote Delsys Trigno systems. 
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_CLIENT_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_CLIENT_HPP_

#include <string>
#include <vector>
#include "duration.hpp"          // trigno::Duration
#include "interface.hpp"         // trigno::network::Interface
#include "configuration.hpp"     // trigno::network::MultiSensorConfiguration, ...
#include "emg_data_client.hpp"   // trigno::network::EMGDataClient
#include "aux_data_client.hpp"   // trigno::network::AUXDataClient

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Class that handles bidirectional network communication with a Trigno SDK server instance (i.e. Trigno Control Application (TCA)).
///
/// @note       Intended to remove explicit dependency on boost:: classes when listening to the server & implementing high level system management (start/stop, configuration, etc)
///             the network protocol describe in the <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
///             EMG & AUX data parsing is handled by specialized classes (cf. DataClient).
///
class Client {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (idle) instance.
    ///
    Client();

    //--------------------------------------------------------------------------
    /// @brief      Constructs & initializes a new instance.
    ///
    /// @param[in]  address        Address (IP) of the Trigno server (machine running the Trigno Control Application (TCA)).
    /// @param[in]  command_port   Command port. Defaults to Trigno's defaults.
    /// @param[in]  emg_data_port  EMG data port. Defaults to Trigno's defaults.
    /// @param[in]  aux_data_port  AUX data port. Defaults to Trigno's defaults.
    /// @param[in]  timeout        Timeout value (s) to abort connection.
    ///
    explicit Client(const std::string& address, size_t command_port    = ConnectionConfiguration::COMMAND_PORT,
                                                size_t emg_data_port   = ConnectionConfiguration::EMG_DATA_PORT,
                                                size_t aux_data_port   = ConnectionConfiguration::AUX_DATA_PORT,
                                                const Duration& timeout = Duration(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~Client();

    //--------------------------------------------------------------------------
    /// @brief      Checks if client is currently connected.
    ///
    /// @return     True if connected to all trigno data channels (command, EMG and Aux), false otherwise.
    ///
    bool connected() const;

    //--------------------------------------------------------------------------
    /// @brief      Initialize client (connect to server & fetch system configuration)
    ///
    /// @param[in]  address        Address (IP) of the Trigno server (machine running the Trigno Control Application (TCA)).
    /// @param[in]  command_port   Command port. Defaults to Trigno's defaults.
    /// @param[in]  emg_data_port  EMG data port. Defaults to Trigno's defaults.
    /// @param[in]  aux_data_port  AUX data port. Defaults to Trigno's defaults.
    /// @param[in]  timeout        Timeout value (ms) to abort connection.
    ///
    void initialize(const std::string& address, size_t command_port  = ConnectionConfiguration::COMMAND_PORT,
                                                size_t emg_data_port = ConnectionConfiguration::EMG_DATA_PORT,
                                                size_t aux_data_port = ConnectionConfiguration::AUX_DATA_PORT,
                                                const Duration& timeout = Duration(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Disconnects from server, closing underlying network (TCP/IP) clients.
    ///
    void shutdown();

    //--------------------------------------------------------------------------
    /// @brief      EMG data client, hadling incoming data streamed from a Trigno SDK server.
    ///
    EMGDataClient EMG;

    //--------------------------------------------------------------------------
    /// @brief      AUX data client, hadling incoming data streamed from a Trigno SDK server.
    ///
    AUXDataClient AUX;

    //--------------------------------------------------------------------------
    /// @brief      Network communication handler (bidirectional), implementing basic query & command structure with a Trigno SDK server.
    ///
    Interface server;

    //--------------------------------------------------------------------------
    /// @brief      System control handler, wrapping around query & command interface to provide high-level Trigno system control.
    ///
    SystemControl system;

    //--------------------------------------------------------------------------
    /// @brief      Base information accessor, wrapping around query & command interface to fetch physical (non-configurable) properties of
    ///             the Trigno base station.
    ///
    BaseInformation base;

    //--------------------------------------------------------------------------
    /// @brief      Sensor configuration handler, wrapping around query & command interface to provide high-level Trigno sensor parametrization.
    ///
    /// @note       LabelledSet class template provides an intuitive interface to handle multiple labelled sensors.
    ///
    ConnectionConfiguration connection;

    //--------------------------------------------------------------------------
    /// @brief      Sensor configuration handler, wrapping around query & command interface to provide high-level Trigno sensor parametrization.
    ///
    /// @note       std::close_index<> is an STL-like container adapter that provides high-level labelling functionalities over a std::vector.
    ///             Allows a more intuitive interface when handle multiple labelled sensors.
    ///
    MultiSensorConfiguration sensors;
};

}  // namespace trigno::network

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_CLIENT_HPP_
