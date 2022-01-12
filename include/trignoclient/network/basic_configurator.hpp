#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICCONFIGURATOR_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICCONFIGURATOR_HPP_

#include <string>

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Forward declaration of trigno::network::Interface in order to avoid cyclic dependency.
///
class Interface;



//------------------------------------------------------------------------------
/// @brief      Abstract base class describing an active configurator, able to communicate with a remote Trigno SDK server for fetching and modifying hardware configuration.
///
/// @note       Allows defining base interface for different types of configuration (e.g. Trigno base station, different sensor types, network-specific global parameters, etc.)
///
/// @note       Wraps around an existing interface/network handler, therefore does not own any connection. Alternatively, one could have each configurator inherit directly from Interface,
///             but that would imply multiple active connections with the Trigno SDK server without a real need, potentially adding server overhead.
///
class BasicConfigurator {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      network  Network handler/interface. Must provide InterfaceType::query() members to allow bidirectional message exchange.
    ///
    explicit BasicConfigurator(Interface* network);

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~BasicConfigurator();

    //--------------------------------------------------------------------------
    /// @brief      Resets the configuration to default values.
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///
    virtual void reset() = 0;

    //--------------------------------------------------------------------------
    /// @brief      Get configuration from Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///
    virtual bool get() = 0;

    //--------------------------------------------------------------------------
    /// @brief      Set hardware configuration into Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///
    virtual bool set() = 0;

    //--------------------------------------------------------------------------
    /// @brief      Attaches configurator to given interface.
    ///
    /// @param      interface  The interface
    ///
    void attachTo(Interface* interface);

    //--------------------------------------------------------------------------
    /// @brief      Custom conversion operator to text string.
    ///
    /// @note       Useful to collapse all configuration into a single string that can be sent to the SDK server for bulk processing.
    ///             Can also be used to get visual feedback of configuration content.
    ///
    virtual operator std::string() const = 0;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Network handler.
    ///
    Interface* _network;
};

}  // namespace trigno::network

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICCONFIGURATOR_HPP_
