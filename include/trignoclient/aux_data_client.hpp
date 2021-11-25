#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EMGDATACLIENT_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EMGDATACLIENT_HPP_

#include <string>
#include "basic_data_client.hpp"    // trigno::network::BasicDataClient
#include "configuration.hpp"        // trigno::network::MultiSensorConfiguration, trigno::network::ConnectionConfiguration::EMG_DATA_CHANNEL
#include "data_frame.hpp"           // trigno::DataFrame

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Unidirectional client for Trigno sensors' AUX data channels.
///
class AUXDataClient: public BasicDataClient {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  server_address   Address of the machine running TCA
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames, which are configuration-dependent.
    /// @param[in]  emg_port         EMG data channel port (defaults to EMG_DATA_CHANNEL).
    /// @param[in]  connect_timeout  Timeout value (ms) to abort connection.
    ///
    AUXDataClient(const std::string& server_address,
                  const MultiSensorConfiguration* configuration,
                  int aux_port = ConnectionConfiguration::EMG_DATA_CHANNEL,
                  int connect_timeout = 100);

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~AUXDataClient();

    //--------------------------------------------------------------------------
    /// @brief      Reads from Trigno server data channel and parses read content into an DataFrame.
    ///
    /// @param[in]  timeout_ms  Timeout value, in milliseconds. Defaults to 15.
    ///
    /// @throws     std::runtime_error if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    DataFrame read(size_t timeout_ms = 15) override;

    //--------------------------------------------------------------------------
    /// @brief      Reads data while populating an existing *DataFrame* instabce with updated values.
    ///
    /// @param[out] out         Output DataFrame instance.
    /// @param[in]  timeout_ms  Read timeout, in milliseconds. Defaults to 15.
    ///
    /// @note       Sensor configuration is ignored, but *out* needs to match system configuration, otherwise data mismatch will occur.
    ///
    /// @note       Useful when configuration is unlikely to change, and same object can be reused (faster).
    ///
    /// @throws     std::runtime_error  if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    void read(DataFrame* out, size_t timeout_ms = 15) override;
};



//--------------------------------------------------------------------------
/// @brief      Generic type describing 3D coordinates or axial values ('x', 'y' and 'z').
///             Useful when representing linear/spatial quatities.
///
struct Coordinates3D {
    //--------------------------------------------------------------------------
    /// @brief      Value along the 'X' axis .
    ///
    float x;

    //--------------------------------------------------------------------------
    /// @brief      Value along the 'Y' axis.
    ///
    float y;

    //--------------------------------------------------------------------------
    /// @brief      Value along the 'Z' axis.
    ///
    float z;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  x     'X' value.
    /// @param[in]  y     'Y' value.
    /// @param[in]  z     'Z' value.
    ///
    explicit Coordinates3D(float X = 0.0, float Y = 0.0, float Z = 0.0);
};



//------------------------------------------------------------------------------
/// @brief      Generic type describing inertial data acquired with an IMU (or similar).
///             Useful when representing auxiliary data frames.
///
struct Inertia {
    //--------------------------------------------------------------------------
    /// @brief      Accelerometer readings.
    ///
    Coordinates3D acc;

    //--------------------------------------------------------------------------
    /// @brief      Gyroscope readings.
    ///
    Coordinates3D gyro;

    //--------------------------------------------------------------------------
    /// @brief      Magnetomoter readings.
    ///
    Coordinates3D mag;
};


}  // namespace trigno::network

#endif  // TRIGGNOCLIENT_INCLUDE_TRIGNOCLIENT_EMGDATACLIENT_HPP_
