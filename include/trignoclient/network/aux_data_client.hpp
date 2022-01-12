//------------------------------------------------------------------------------
/// @file       aux_data_client.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of trigo::network::AUXDataClient, the specialized data client that receives and parses
///             Auxiliary (inertial, force-sesing, etc.) data from remote Delsys Trigno systems.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_AUXDATACLIENT_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_AUXDATACLIENT_HPP_

#include <string>
#include "core/duration.hpp"                // trigno::Duration
#include "core/frame.hpp"                   // trigno::Frame
#include "network/basic_data_client.hpp"    // trigno::network::BasicDataClient
#include "network/configuration.hpp"        // trigno::network::MultiSensorConfiguration, trigno::network::ConnectionConfiguration::EMG_DATA_CHANNEL

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Unidirectional client for Trigno sensors' AUX data channels.
///
class AUXDataClient: public BasicDataClient {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (idle) instance.
    ///
    /// @param[in]  channels         Number of data channels.
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames/samples, which are configuration-dependent.
    ///
    explicit AUXDataClient(MultiSensorConfiguration* configuration);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance & connects to Trigno TCP server.
    ///
    /// @param[in]  channels         Number of data channels.
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames, which are configuration-dependent.
    /// @param[in]  address          Address of the machine running TCA
    /// @param[in]  emg_data_port    EMG data channel port.
    /// @param[in]  timeout          Timeout value (s) to abort connection.
    ///
    explicit AUXDataClient(MultiSensorConfiguration* configuration, const std::string& address, size_t emg_data_port = ConnectionConfiguration::EMG_DATA_PORT, const Duration& timeout = Duration(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Establishes a connection (TCP/IP) to a Trigno server @*server_address*.
    ///
    /// @param[in]  address          Address of the machine running TCA
    /// @param[in]  emg_data_port    EMG data channel port (defaults to EMG_DATA_CHANNEL).
    /// @param[in]  timeout          Timeout value (s) to abort connection.
    ///
    /// @throws     std::runtime_error if connection is not possible (no Trigno server @*address* or timeout).
    ///
    /// @note       Overload provided in order to override default data port value.
    ///
    void connect(const std::string& address, size_t port = ConnectionConfiguration::EMG_DATA_PORT, const Duration& timeout = Duration(ConnectionConfiguration::CONNECT_TIMEOUT)) override;

    //--------------------------------------------------------------------------
    /// @brief      Resets client status (frame counter, sensor configuration and sample rate).
    ///
    void reset() override;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Builds an EMG data frame for given *sensors* from data buffer.
    ///             Requires buffer to be updated/filled externally.
    ///
    /// @param[in]  sensors  Sensors which resulting frame holds data from.
    ///                      Only those which are active are considered, and all other remaining sensors are ignored.
    ///                      Defaults to complete sensor set i.e. reads/parses data for all active sensors.
    ///
    /// @return     Instance of trigno::Frame
    ///
    Frame buildFrame(const sensor::List& sensors = sensor::all) const override;
};

}  // namespace trigno::network

#endif  // TRIGGNOCLIENT_INCLUDE_TRIGNOCLIENT_AUXDATACLIENT_HPP_

