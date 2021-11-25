#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EMGDATACLIENT_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EMGDATACLIENT_HPP_

#include <string>
#include "basic_data_client.hpp"    // trigno::network::BasicDataClient
#include "configuration.hpp"        // trigno::network::MultiSensorConfiguration, trigno::network::ConnectionConfiguration::EMG_DATA_CHANNEL
#include "frame.hpp"                // trigno::Frame

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Unidirectional client for Trigno sensors' EMG data channels.
///
class EMGDataClient: public BasicDataClient {
 public:
    using BasicDataClient::Timeout;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (idle) instance.
    ///
    /// @param[in]  channels         Number of data channels.
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames/samples, which are configuration-dependent.
    ///
    explicit EMGDataClient(MultiSensorConfiguration* configuration);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance & connects to Trigno TCP server.
    ///
    /// @param[in]  channels         Number of data channels.
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames, which are configuration-dependent.
    /// @param[in]  address          Address of the machine running TCA
    /// @param[in]  emg_data_port    EMG data channel port.
    /// @param[in]  timeout          Timeout value (s) to abort connection.
    ///
    explicit EMGDataClient(MultiSensorConfiguration* configuration, const std::string& address, size_t emg_data_port = ConnectionConfiguration::EMG_DATA_PORT, const Timeout& timeout = Timeout(ConnectionConfiguration::CONNECT_TIMEOUT));

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
    void connect(const std::string& address, size_t port = ConnectionConfiguration::EMG_DATA_PORT, const Timeout& timeout = Timeout(ConnectionConfiguration::CONNECT_TIMEOUT)) override;

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

#endif  // TRIGGNOCLIENT_INCLUDE_TRIGNOCLIENT_EMGDATACLIENT_HPP_
