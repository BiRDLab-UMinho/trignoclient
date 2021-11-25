#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_INTERFACE_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_INTERFACE_HPP_

#include <string>
#include <thread>
#include "std/tcp_client.hpp" // std::tcp_client
#include "std/duration.hpp"   // std::duration
#include "configuration.hpp"  // trigno::network::ConnectionConfiguration

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Class implementing simple query & command protocols for bidirectional communication with a Trigno SDK server.
///
/// @note       Establishes connection with Trigno SDK command channel. To avoid multiple parallel connections,
///             develop wrappers around a Interface instance vs deriving from the class.
///
class Interface {
 public:
    //--------------------------------------------------------------------------
    /// @brief      TCP handler type (for code readablity).
    ///
    using Client = std::tcp_client;

    //--------------------------------------------------------------------------
    /// @brief      Buffer type (for code readablity).
    ///
    using Buffer = std::tcp_client::dynamic_buffer;

    //--------------------------------------------------------------------------
    /// @brief      Duration/timeout type.
    ///
    using Timeout = std::duration;

    //--------------------------------------------------------------------------
    /// @brief      Command termination sequence.
    ///
    /// @note       Specific to Trigno network protocol, as described in <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
    ///
    static const std::string termination_word;  // = "\r\n";

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (uninitialized) instance.
    ///
    Interface() = default;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance & connect to Trigno server.
    ///
    /// @param[in]  address  Network address of the maching running the Trigno server.
    /// @param[in]  port     Server port/service to connect to. Defaults to Trigno's command port.
    /// @param[in]  timeout  Timeout value (s) to abort connection.
    ///
    explicit Interface(const std::string& address, size_t port = ConnectionConfiguration::COMMAND_PORT,
                                                   const Timeout& timeout = Timeout(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~Interface() = default;

    //--------------------------------------------------------------------------
    /// @brief      Access protocol version.
    ///
    /// @return     Text descriptor of the Trigno SDK version.
    ///
    /// @throw      std::runtime_error if there is no active connection.
    ///
    const std::string& version() const;

    //--------------------------------------------------------------------------
    /// @brief      Establishes a connection (TCP/IP) to a Trigno server @*server_address*
    ///
    /// @param[in]  address  Address of the machine running TCA.
    /// @param[in]  port     Data channel service/port. Cf. Trigno User Manual for default port allocation.
    /// @param[in]  timeout  Timeout value (s) to abort connection.
    ///
    /// @throws     std::runtime_error if connection is not possible (no Trigno server @*address* or timeout).
    ///
    virtual void connect(const std::string& address, size_t port = ConnectionConfiguration::COMMAND_PORT,
                                                     const Timeout& timeout = Timeout(ConnectionConfiguration::CONNECT_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Closes the connection (TCP/IP) to a Trigno server @*server_address*, if connected.
    ///
    void disconnect();

    //--------------------------------------------------------------------------
    /// @brief      Sends a generic query/command to the TCA.
    ///
    /// @param[in]  query     Text query
    /// @param[out] response  Query's response. If nullptr, query() does not wait for server response.
    /// @param[in]  timeout   Timeout value (ms) to abort write and receive.
    /// @param[in]  delay     Time delay (ms) before sending query to server.
    ///
    /// @throws     std::runtime_error     if timeout is exceed or connection to TCA is lost while waiting for data.
    /// @throws     std::invalid_argument  if 'INVALID COMMAND' or 'CANNOT COMPLETE' is returned by the TCA
    ///
    /// @note       Can be used to send commands as well, but may require manual check for success ('OK') in response.
    ///
    /// @todo       Move delay argument to base class basic_tcp_client?
    ///
    void query(const std::string& query, std::string* response, const Timeout& timeout = Timeout(ConnectionConfiguration::IO_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Sends a generic query/command to the TCA.
    ///
    /// @param[in]  query     Text query
    /// @param[in]  timeout   Timeout value (s) to abort write and receive.
    /// @param[in]  delay     Time delay (s) before sending query to server.
    ///
    /// @return     Response of the Trigno server to given query.
    ///
    /// @throw      std::runtime_error if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    /// @throw      std::invalid_argument if 'INVALID COMMAND' or 'CANNOT COMPLETE' is returned by the TCA
    ///
    std::string query(const std::string& query, const Timeout& timeout = Timeout(ConnectionConfiguration::IO_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Specialized TCA query for command prompts with success check on return.
    ///
    /// @param[in]  command            Text query
    /// @param[in]  success_response   Text string which server's response should match for a 'True' return value.
    /// @param[in]  timeout            Timeout value (s) to abort write and receive.
    /// @param[in]  delay              Time delay (s) before sending query to server.
    ///
    /// @return     True if TCA returns *success_response*, False otherwise.
    ///
    /// @throw      std::runtime_error if timeout is exceed or connection to TCA is lost while waiting for data.
    ///
    /// @throw      std::invalid_argument if 'INVALID COMMAND' or 'CANNOT COMPLETE' is returned by the TCA
    ///
    bool command(const std::string& command, const std::string& success_response = "", const Timeout& timeout = Timeout(ConnectionConfiguration::IO_TIMEOUT));

    //--------------------------------------------------------------------------
    /// @brief      Waits for a specific response from TCA.
    ///
    /// @param[in]  target       Response to wait for.
    /// @param[in]  timeout      Read timeout (s).
    /// @param[in]  max_attempts Maximum number of attempts, to prevent indefinite blocking.
    ///
    /// @return     True if *response* was received, false otherwise.
    ///
    bool waitFor(const std::string& target, const Timeout& timeout = Timeout(ConnectionConfiguration::IO_TIMEOUT), size_t max_attempts = 1000);

    //--------------------------------------------------------------------------
    /// @brief      Schedule (member) function call after given *delay*.
    ///             
    /// @param[in]  delay  Delay with which to call given *func*.
    /// @param[in]  func   Member function to call
    /// @param[in]  args   Arguments to given *func*
    ///
    /// @tparam     T      Return type of *func*
    /// @tparam     Args   Variadic parameter packwi
    ///
    /// @return     std::future< T > instance holding result of function call.
    ///
    template < typename T, typename... Args >
    std::future< T > schedule(const Timeout& delay, const std::function< T(Interface, Args...) >& func, Args&&... args);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Network protocol descriptor (i.e. version), provided by the Trigno SDK server.
    ///
    std::string _protocol_version;

    //--------------------------------------------------------------------------
    /// @brief      Network communication handler.
    ///
    Client _network;

    //--------------------------------------------------------------------------
    /// @brief      Dynamic buffer to hold incoming server responses.
    ///
    Buffer _buffer;
};



template < typename T, typename... Args >
std::future< T > Interface::schedule(const Timeout& delay, const std::function< T(Interface, Args...) >& func, Args&&... args) {
    std::future< T > ret =  std::async(std::launch::async, [delay, this, func, args... ] {
        std::this_thread::sleep_for(delay);
        func(std::forward< Args >(args)...);
    });
    // , func, std::forward< Args >(args)...);
    return ret;
}

}  // namespace trigno::network

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_INTERFACE_HPP_
