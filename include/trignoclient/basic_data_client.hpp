//------------------------------------------------------------------------------
/// @file       basic_data_client.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of trigo::BasicDataClient class, implementing unidirectional data 
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICDATACLIENT_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICDATACLIENT_HPP_

#include <string>
#include <vector>
#include "std/tcp_client.hpp"  // std::tcp_client
#include "duration.hpp"        // trigno::Duration
#include "configuration.hpp"   // trigno::network::MultiSensorConfiguration
#include "frame.hpp"           // trigno::Frame
#include "sequence.hpp"        // trigno::Sequence

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Class describing a generic data client that reads incoming fixed-size data packets from a TCP/IP server.
///
/// @note       Generic class template with no Trigno SDK depencies, thus declared outside trigno:: namespace.
///
/// @todo       Move tcp_handler stuff into this class, no need for a separate base class? or derive from it, declaring unneded members as private!
///
class BasicDataClient {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Tupe of the values being parsed from network stream.
    ///
    using DataValue = float;

    //--------------------------------------------------------------------------
    /// @brief      TCP handler type
    ///
    using Client =  std::tcp_client;

    //--------------------------------------------------------------------------
    /// @brief      Convenience buffer alias(es).
    ///
    using Buffer = std::tcp_client::dynamic_buffer;
    // using Buffer = std::tcp_client::static_buffer< sizeof(std::declval< Value >()) /* == 4 bytes */ >;

    //--------------------------------------------------------------------------
    /// @brief      Timeout/duration type. Specialization of std::chrono::seconds<> for float values.
    ///
    /// @note       Compatible std::chrono literals (cf. https://en.cppreference.com/w/cpp/chrono/operator%22%22s).
    ///
    using Timeout = std::chrono::milliseconds;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (idle) instance.
    ///
    /// @param[in]  channels         Number of data channels.
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames, which are configuration-dependent.
    ///                              Default to nullptr, in cases where derived data clients do not require access to sensor configuration.
    ///
    explicit BasicDataClient(size_t channels, MultiSensorConfiguration* configuration = nullptr);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance & connects to Trigno TCP server.
    ///
    /// @param[in]  channels         Number of data channels.
    /// @param[in]  configuration    Sensor configuration instance. Required for creating new data frames, which are configuration-dependent.
    /// @param[in]  address          Address of the machine running TCA.
    /// @param[in]  port             Data channel service/port. Cf. Trigno User Manual for default port allocation.
    /// @param[in]  timeout          Timeout value (ms) to abort connection.
    ///
    explicit BasicDataClient(size_t channels, MultiSensorConfiguration* configuration, const std::string& address, size_t port, const Duration& timeout = Duration(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~BasicDataClient();

    //--------------------------------------------------------------------------
    /// @brief      Checks if currently connected to Trigno command server.
    ///
    /// @return     True if connected to Trigno command channel, false otherwise.
    ///
    bool connected() const;

    //--------------------------------------------------------------------------
    /// @brief      Establishes a connection (TCP/IP) to a Trigno server @*server_address*
    ///
    /// @param[in]  address          Address of the machine running TCA.
    /// @param[in]  port             Data channel service/port. Cf. Trigno User Manual for default port allocation.
    /// @param[in]  timeout          Timeout value (ms) to abort connection.
    ///
    /// @throws     std::runtime_error if connection is not possible (no Trigno server @*address* or timeout).
    ///
    virtual void connect(const std::string& address, size_t port, const Duration& timeout = Duration(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Closes the connection (TCP/IP) to a Trigno server @*server_address*, if connected.
    ///
    virtual void disconnect();

    //--------------------------------------------------------------------------
    /// @brief      Resets client status (frame counter, sensor configuration)
    ///
    /// @note       Base implementation assumes a null sample rate i.e. time stamps are kept to 0.0 unless derived types fetch a data-specific value. 
    ///
    virtual void reset();

    //--------------------------------------------------------------------------
    /// @brief      Reads from Trigno server data channel and parses read content into an Frame .
    ///
    /// @param[in]  timeout  Timeout value, in milliseconds. Defaults to 15.
    /// @param[in]  sensors  Sensors which resulting frame holds data from.
    ///                      Only those which are active are considered, and all other remaining sensors are ignored.
    ///                      Defaults to complete sensor set i.e. reads/parses data for all active sensors.
    ///
    /// @tparam     T        Output frame type, either Frame or Frame::Stamped (default).
    ///
    /// @throws     std::runtime_error if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///             Base class is agnostic as to how to strucut a data frame (e.g. EMG vs Aux data)
    ///
    template < typename T = Frame::Stamped >
    T read(const sensor::List& sensors = sensor::all, const Duration& timeout = Duration(ConnectionConfiguration::IO_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Reads data while populating an existing *Frame* instabce with updated values.
    ///
    /// @param[out] out      Output Frame instance to assign.   
    /// @param[in]  sensors  Sensors which resulting frame holds data from.
    ///                      Only those which are active are considered, and all other remaining sensors are ignored.
    ///                      Defaults to complete sensor set i.e. reads/parses data for all active sensors.
    /// @param[in]  timeout  Read timeout, in milliseconds. Defaults to 15.
    ///
    /// @tparam     T        Frame type, either Frame or Frame::Stamped (default).
    ///
    /// @note       Useful when configuration is unlikely to change, and same object can be reused (faster).
    ///
    /// @throws     std::runtime_error  if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    template < typename T = Frame::Stamped >
    void read(T* out, const sensor::List& sensors = sensor::all, const Duration& timeout = Duration(ConnectionConfiguration::IO_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Reads data directly into an existing *Frame* instance.
    ///
    /// @param[out] out      Output data frame. Contents are overwritten.
    ///
    /// @tparam     T        Frame type, either Frame or Frame::Stamped (default).
    ///
    /// @throws     std::runtime_error  if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    template < typename T = Frame::Stamped >
    void operator>>(T& out);

    //--------------------------------------------------------------------------
    /// @brief      Reads data directly into the back of an existing *Sequence* instance.
    ///
    /// @param[out] sequence    Output data frame sequence. A new frame is added to the back of the queue.
    ///
    virtual void operator>>(Sequence& sequence);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Builds a data frame for given *sensors* from data buffer.
    ///             Requires buffer to be updated/filled externally.
    ///
    /// @param[in]  sensors  Sensors which resulting frame holds data from.
    ///                      Only those which are active are considered, and all other remaining sensors are ignored.
    ///                      Defaults to complete sensor set i.e. reads/parses data for all active sensors.
    ///
    /// @return     Newly created instance of trigno::Frame.
    ///
    /// @note       Pure virtual, derived data clients must provide their own implementation.
    ///
    virtual Frame buildFrame(const sensor::List& sensors = sensor::all) const = 0;

    //--------------------------------------------------------------------------
    /// @brief      Network communication handler.
    ///
    Client _network;

    //--------------------------------------------------------------------------
    /// @brief      Static data buffer.
    ///
    Buffer _buffer;

    //--------------------------------------------------------------------------
    /// @brief      Current sensor configuration.
    ///
    /// @note       Required to create a full Frame when reading.
    ///
    MultiSensorConfiguration* _configuration;

    //--------------------------------------------------------------------------
    /// @brief      Index of the current/last data_frame, used to compute frame time stamp.
    ///
    size_t _frame_idx;

    //--------------------------------------------------------------------------
    /// @brief      Data channels sample rate, used to compute frame time stamp.
    ///
    float _sample_rate;
};

}  // namespace trigno::network

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICDATACLIENT_HPP_
