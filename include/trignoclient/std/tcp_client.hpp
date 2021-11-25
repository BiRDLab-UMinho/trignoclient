//------------------------------------------------------------------------------
/// @file       tcp_client.hpp
/// @author     João André
///
/// @brief      Header file providing declaration & definition of std::tcp_client for simple TCP/IP communication.
///
/// std::tcp_client employs boost::asio to provide abstraction over network communication with a remote/local TCP server.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_TCPCLIENT_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_TCPCLIENT_HPP_

#include <string>
#include <vector>
#include <limits>
#include <boost/bind/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "std/duration.hpp"     // std::duration

namespace std {

//------------------------------------------------------------------------------
/// @brief      This class describes a generic TCP client providing blocking asynchronous read/write operations with timeout/timeouts,
///             into static & dynamic buffers.
///
/// @note       General use (not Trigno-specific), thus declared outside trigno::network namespace and formatted to comply with boost coding standards.
///
/// @note       Wraps around asynchronous calls while forcibly blocking execution until timeout is expired.
///
/// @note       Inspired from https://www.boost.org/doc/libs/1_52_0/doc/html/boost_asio/example/timeouts/blocking_tcp_client.cpp.
///
class tcp_client {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Timer object type. An high_resolution_timer is used for compatiblity with chrono (& chrono literals).
    ///
    using timer = boost::asio::high_resolution_timer;

    //--------------------------------------------------------------------------
    /// @brief      Convenience alias of socket object type.
    ///
    using socket = boost::asio::ip::tcp::socket;

    //--------------------------------------------------------------------------
    /// @brief      Convenience alias of error/exception type.
    ///
    using error_code = boost::system::error_code;

    //--------------------------------------------------------------------------
    /// @brief      Convenience alias of container type used in dynamic buffer operations (cf. tcp_client::read_until(), write()).
    ///
    using dynamic_buffer = vector< char >;

    //--------------------------------------------------------------------------
    /// @brief      Convenience alias of container type used in static buffer operations (cf. tcp_client::read(), write()).
    ///
    template < int N >
    using static_buffer = boost::array< unsigned char, N >;

    //--------------------------------------------------------------------------
    /// @brief      Infinite timeout descriptor.
    ///
    // static constexpr duration infinite = duration(std::numeric_limits< size_t >::max());

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    tcp_client();

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~tcp_client();

    //--------------------------------------------------------------------------
    /// @brief      Checks if client is currently connected to a server.
    ///
    /// @return     True if connected, False otherwise.
    ///
    bool is_connected() const;

    //--------------------------------------------------------------------------
    /// @brief      Connect to a remote TCP host.
    ///
    /// @param[in]  address  Host address
    /// @param[in]  port     Host port/service
    /// @param[in]  timeout  Timeout/timeout value. Defaults to 1 second.
    ///
    void connect(const string& address, int port, const duration& timeout = duration(1.0 /* seconds */));

    //--------------------------------------------------------------------------
    /// @brief      Shutsdown connection, closing the socket in the process.
    ///
    void disconnect();

    //--------------------------------------------------------------------------
    /// @brief      Sets timer timeout and resets error status.
    ///
    /// @param[in]  timeout  Timeout/timeout value. Defaults to infinite (no timeout).
    ///
    void reset(const duration& timeout = duration::max());

    //--------------------------------------------------------------------------
    /// @brief      Runs IO service on a loop, until all current asynchronous operations are completed or timeout is expired.
    ///
    void run();

    //--------------------------------------------------------------------------
    /// @brief      Read a fixed-size character sequence from remote host, into a pre-allocated buffer.
    ///
    /// @param[out] buffer    Container to hold read data.
    /// @param[in]  timeout  Timeout/timeout value. Defaults to 1 second.
    ///
    /// @tparam     Buffer  Output container type.
    ///             Used instead of member type static_buffer<> in order to allow the use of other buffers (e.g. std::array)
    ///             Must be compatible with boost::asio::buffer()
    ///
    /// @return     True if read was successful, false otherwise.
    ///
    /// @note       Static/fixed-size buffer variant, useful when message length is already known/constant.
    ///
    /// @note       Buffer must be compatible with boost::asio::buffer() -> boost::array, std::array, std::vector, etc.
    ///
    template < typename Buffer >
    void read(Buffer& buffer, const duration& timeout = duration(1.0 /* seconds */));

    //--------------------------------------------------------------------------
    /// @brief      Read a fixed-size character sequence from remote host.
    ///
    /// @param[in]  timeout  Timeout/timeout value. Defaults to 1 second.
    ///
    /// @tparam     Size  Size of static buffer/Number of bytes to read from stream.
    ///
    /// @return     Static buffer/array instance with *Size* elements.
    ///
    /// @note       Static/fixed-size buffer variant, useful when message length is already known/constant.
    ///
    template < int Size >
    static_buffer< Size > read(const duration& timeout = duration(1.0 /* seconds */));

    //--------------------------------------------------------------------------
    /// @brief      Reads a variable character sequence from remote host, into a pre-allocated buffer.
    ///
    /// @param[in]  stop      Message delimiter/stop character.
    /// @param[out] buffer    Container to hold read data.
    /// @param[in]  timeout  Timeout/timeout value. Defaults to 1 second.
    ///
    /// @tparam     Buffer  Output container type.
    ///             Used instead of member type static_buffer<> in order to allow the use of other buffers (e.g. std::vector, std::string)
    ///             Must be compatible with boost::asio::dynamic_buffer()
    ///
    /// @return     True if read was successful, false otherwise.
    ///
    template < typename Buffer >
    void read_until(char stop, Buffer& buffer, const duration& timeout = duration(1.0 /* seconds */));

    //--------------------------------------------------------------------------
    /// @brief      Reads a variable character sequence from remote host.
    ///
    /// @param[in]  stop      Message delimiter/stop character.
    /// @param[in]  timeout  Timeout/timeout value. Defaults to 1 second.
    ///
    /// @return     Vector of byte/char values.
    ///
    /// @note       Dynamic buffer variant, useful when message length is unknown (a stop character is required).
    ///
    dynamic_buffer read_until(char stop, const duration& timeout = duration(1.0 /* seconds */));

    //--------------------------------------------------------------------------
    /// @brief      Writes a character sequence to the remote host.
    ///
    /// @param[in]  buffer    Container with data to write.
    /// @param[in]  timeout  Timeout/timeout value. Defaults to 1 second.
    ///
    /// @tparam     Buffer  Output container type. Must be compatible with boost::asio IO operations.
    ///
    /// @note       Does not add any stop character. If the communication protocol requires, take care to add it at the end of buffer.
    ///
    template < typename Buffer >
    void write(const Buffer& buffer, const duration& timeout = duration(1.0 /* seconds */));

    //--------------------------------------------------------------------------
    /// @brief      Checks if timeout has been reached.
    ///
    /// @return     True if timeout has expired, false otherwise.
    ///
    /// @note       Updates error code with result of socket close operation if timeout has been reached.
    ///
    bool check_timeout();

    //--------------------------------------------------------------------------
    /// @brief      Generic error handler, which updates shared member error code and checks for abort conditions.
    ///
    /// @param[in]  error   boost::asio error code, returned by IO asynchronous operations.
    /// @param[in]  length  Number of bytes written/read @ IO opertion exit.
    ///
    /// @throws     boost::asio::error::operation_aborted   if socket is closed prematurely but IO operation succeeds.
    /// @throws     boost::asio::error::eof                 if IO operation fails while reading from remote host.
    /// @throws     boost::asio::error::...                 if socket ...
    ///
    void check_status();

    //--------------------------------------------------------------------------
    /// @brief      Connect handler (called after successful connect operation).
    ///
    /// @param[in]  error   boost::asio error code, returned by IO asynchronous operations.
    ///
    void handle_connect(const error_code& error);

    //--------------------------------------------------------------------------
    /// @brief      Read handler (called after successful read operations).
    ///
    /// @param[in]  error   boost::asio error code, returned by IO asynchronous operations.
    /// @param[in]  length  Number of bytes written/read @ exit.
    ///
    void handle_read(const error_code& error, size_t length);

    //--------------------------------------------------------------------------
    /// @brief      Write handler (called after successful write operations).
    ///
    /// @param[in]  error   boost::asio error code, returned by IO asynchronous operations.
    /// @param[in]  length  Number of bytes written/read @ exit.
    ///
    void handle_write(const error_code& error, size_t length);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      IO service handling read/write from streams/sockets
    ///
    boost::asio::io_context _io;

    //--------------------------------------------------------------------------
    /// @brief      Timer tracking timeout to operation abort.
    ///
    timer _timer;

    //--------------------------------------------------------------------------
    /// @brief      Error code, shared between member functions.
    ///
    error_code _error;

    //--------------------------------------------------------------------------
    /// @brief      Socket instance to handle IO operations
    ///
    socket _socket;
};



inline tcp_client::tcp_client() :
    // _io(),
    _timer(_io),
    _socket(_io) {
        // ...
        // initialize timeout to infinity (null) -> not necessary until read/write functions are called
        // _timer.expires_from_now(duration::max);
        reset();

        // start the persistent actor that checks for timeout expiry.
        check_timeout();
}



inline tcp_client::~tcp_client() {
    /* ... */
    disconnect();
}



inline bool tcp_client::is_connected() const {
    return _socket.is_open();
}



inline void tcp_client::reset(const duration& timeout) {
    // set timeout for the asynchronous operation.
    // @note: host name may resolve to multiple endpoints (async_connect will *iter* to loop over them), timeout applies to the entire operation
    _timer.expires_from_now(std::chrono::duration_cast< timer::duration >(timeout));

    // reset error state
    // @note *would_block* signals incomplete operations. boost::asio asynchronous calls will never fail with *would_block*
    _error = boost::asio::error::would_block;
}



inline void tcp_client::run() {
    // block until asynchronous operations have completed or timeout has expired.
    // if error code is already different than *would_block*, nothing to do.
    do {
        _io.run_one();
    } while (_error == boost::asio::error::would_block);
}



inline void tcp_client::connect(const std::string& address, int port, const duration& timeout) {
    // resolve host name/port(service) to a list of endpoints.
    boost::asio::ip::tcp::resolver::iterator iter = boost::asio::ip::tcp::resolver(_io).resolve(address, std::to_string(port), _error);

    // // set timeout for the asynchronous operation.
    // // @note: host name may resolve to multiple endpoints (async_connect will *iter* to loop over them), timeout applies to the entire operation
    // _timer.expires_from_now(timeout);

    // // reset error state
    // // @note *would_block* signals incomplete operations. boost::asio asynchronous calls will never fail with *would_block*
    // _error = boost::asio::error::would_block;
    reset(timeout);

    // start the asynchronous operation itself.
    // @note boost::lambda is used to specify a lambda that binds 1st output variable to member _error
    // @note alternatively, a member function handle_connect() can be used (better for extensibility, but needs to call base function on override)
    // async_connect(_socket, iter, boost::lambda::var(_error) = boost::lambda::_1);
    async_connect(_socket, iter, boost::bind(&tcp_client::handle_connect, this, boost::placeholders::_1));

    // block until the asynchronous operation has completed.
    run();

    // exit cases:
    // ec == 0 -> success
    // ec !=0 && ec!= would_block -> completed, failed
    // ec == would_block && !socket.is_open() -> incomplete, timeout expired

    // check whether a connection was successfully established.
    // @note borderline cases: socket may have been forcibly closed (timeout expired) even if asynchronous operation is completed (ec!=woud_block) successfully (_error == 0)
    //       therefore, *operation_aborted* is thrown in  that case (and operation exit error code if unsuccessful with socket still open)
    // @note can/should be done on handle_connect();
    if (_error || !_socket.is_open()) {
        throw boost::system::system_error(_error ? _error : boost::asio::error::operation_aborted);
    }
}



inline void tcp_client::disconnect() {
    // disable internal timer
    reset();  // _timer.expires_at(boost::posix_time::pos_infin);
    // shutdown (required for clean disconnection) & close socket
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, _error);
    _socket.close();
}



template < typename Buffer >
inline void tcp_client::read(Buffer& buffer, const duration& timeout) {
    // // set timeout for the asynchronous operation.
    // // @note: async_read will populate entire buffer, timeout applies to the entire operation (*Size* reads from remote host)
    // _timer.expires_from_now(timeout);

    // // reset error state
    // // @note *would_block* signals incomplete operations. boost::asio asynchronous calls will never fail with *would_block*
    // _error = boost::asio::error::would_block;
    reset(timeout);

    // start the asynchronous operation itself.
    // @note boost::lambda is used to specify a lambda that binds 1st output variable to member _error
    // @note alternatively, a member function handle_read() can be used (better for extensibility, but needs to call base function on override)
    // @note async_read() will throw boost:asio::error::eof if timeout expires before *Size* bytes are read from remote host
    // async_read(_socket, boost::asio::buffer(buffer), boost::asio::transfer_all(), boost::lambda::var(_error) = boost::lambda::_1);
    async_read(_socket, boost::asio::buffer(buffer), boost::asio::transfer_all(), boost::bind(&tcp_client::handle_read, this, boost::placeholders::_1, boost::placeholders::_2));

    // block until the asynchronous operation has completed.
    run();

    // check whether a connection was successfully established.
    // @note borderline cases: socket may have been forcibly closed (timeout expired) even if asynchronous operation is completed (ec!=woud_block) successfully (_error == 0)
    //       therefore, *operation_aborted* is thrown in  that case (and operation exit error code if unsuccessful with socket still open)
    // @note can/should be done on handle_read();
    if (_error || !_socket.is_open()) {
        throw boost::system::system_error(_error ? _error : boost::asio::error::operation_aborted);
    }
}



template < int Size >
inline tcp_client::static_buffer< Size > tcp_client::read(const duration& timeout) {
    static_buffer< Size > buffer;
    read(buffer, timeout);  // delegate to overload implementation
    return buffer;
}



template < typename Buffer >
inline void tcp_client::read_until(char stop, Buffer& buffer, const duration& timeout) {
    // // set timeout for the asynchronous operation.
    // // @note: async_read will populate entire buffer, timeout applies to the entire operation (*Size* reads from remote host)
    // _timer.expires_from_now(timeout);

    // // reset error state
    // // @note *would_block* signals incomplete operations. boost::asio asynchronous calls will never fail with *would_block*
    // _error = boost::asio::error::would_block;
    reset(timeout);

    // start the asynchronous operation itself.
    // @note boost::lambda is used to specify a lambda that binds 1st output variable to member _error
    // @note alternatively, a member function handle_read() can be used (better for extensibility, but needs to call base function on override)
    async_read_until(_socket, boost::asio::dynamic_buffer(buffer), stop, boost::lambda::var(_error) = boost::lambda::_1);
    // async_read_until(_socket, dynamic_buffer(buffer), delimiter, handle_read);

    // block until the asynchronous operation has completed.
    run();

    // check whether a connection was successfully established.
    // @note borderline cases: socket may have been forcibly closed (timeout expired) even if asynchronous operation is completed (ec!=woud_block) successfully (_error == 0)
    //       therefore, *operation_aborted* is thrown in  that case (and operation exit error code if unsuccessful with socket still open)
    // @note can/should be done on handle_read();
    if (_error || !_socket.is_open()) {
        throw boost::system::system_error(_error ? _error : boost::asio::error::operation_aborted);
    }
}



inline tcp_client::dynamic_buffer tcp_client::read_until(char stop, const duration& timeout) {
    dynamic_buffer buffer;
    read_until(stop, buffer, timeout);  // delegate to overload implementation
    return buffer;
}



template < typename Buffer >
inline void tcp_client::write(const Buffer& data, const duration& timeout) {
    // // set timeout for the asynchronous operation.
    // // @note: async_read will populate entire buffer, timeout applies to the entire operation (*Size* reads from remote host)
    // _timer.expires_from_now(timeout);

    // // reset error state
    // // @note *would_block* signals incomplete operations. boost::asio asynchronous calls will never fail with *would_block*
    // _error = boost::asio::error::would_block;
    reset(timeout);

    // start the asynchronous operation itself.
    // @note boost::lambda is used to specify a lambda that binds 1st output variable to member _error
    // @note alternatively, a member function handle_write() can be used (better for extensibility, but needs to call base function on override)
    async_write(_socket, boost::asio::buffer(data), boost::lambda::var(_error) = boost::lambda::_1);
    // async_write(_socket, boost::asio::buffer(data), handle_write);

    // block until the asynchronous operation has completed.
    run();

    // check whether a connection was successfully established.
    // @note borderline cases: socket may have been forcibly closed (timeout expired) even if asynchronous operation is completed (ec!=woud_block) successfully (_error == 0)
    //       therefore, *operation_aborted* is thrown in  that case (and operation exit error code if unsuccessful with socket still open)
    // @note can/should be done on handle_read();
    if (_error || !_socket.is_open()) {
        throw boost::system::system_error(_error ? _error : boost::asio::error::operation_aborted);
    }
}



inline bool tcp_client::check_timeout() {
    // check whether the timeout has passed
    if (_timer.expires_at() <= std::chrono::high_resolution_clock::now()) {
        // if the timeout has expired, socket is closed, aborting any ongoing asynchronous operations.
        // ongoing run() calls will return.
        _socket.close(_error);

        // disable timeout until a new connection is established
        // _timer.expires_at(boost::posix_time::pos_infin);
        reset();

        // throw timed_out exception
        throw boost::asio::error::timed_out;

        return true;
    }

    // if the timeout hasn't been reached, rebind this function as callback to the end of the timer.
    _timer.async_wait(boost::lambda::bind(&tcp_client::check_timeout, this));

    return false;
}



inline void tcp_client::check_status() {
    // check whether a connection was successfully established.
    // @note borderline cases: socket may have been forcibly closed (timeout expired) even if asynchronous operation is completed (ec!=woud_block) successfully (_error == 0)
    //       therefore, *operation_aborted* is thrown (or operation exit error code if unsuccessful with socket still open)
    if (_error || !_socket.is_open()) {
        throw boost::system::system_error(_error ? _error : boost::asio::error::operation_aborted);
    }
}



inline void tcp_client::handle_connect(const boost::system::error_code& error) {
    /* ... */
    // update member variable
    _error = error;
    // generic error parsing, throws exception if connection is not healthy or operation failed
    check_status();
    /* ... */
}



inline void tcp_client::handle_read(const boost::system::error_code& error, size_t length) {
    /* ... */
    // update member variable
    _error = error;
    // generic error parsing, throws exception if connection is not healthy or operation failed
    check_status();
    /* ... */
}



inline void tcp_client::handle_write(const boost::system::error_code& error, size_t length) {
    /* ... */
    // update member variable
    _error = error;
    // generic error parsing, throws exception if connection is not healthy or operation failed
    check_status();
    /* ... */
}

}  // namespace boost::asio::ip

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_TCPCLIENT_HPP_
