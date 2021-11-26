#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_CONFIGURATION_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_CONFIGURATION_HPP_

#include <string>
#include <vector>
#include <iostream>
#include "sensor.hpp"
#include "basic_configurator.hpp"  // trigno::network::BasicConfigurator
#include "std/indexer.hpp"         // std::index<>

namespace trigno::network {

//------------------------------------------------------------------------------
/// @brief      Class describing individual sensor configuration.
///
/// @note       Adds label property in order to provide a customizable high-level interface.
///
/// @note       Stores fetched parametrization internally, in order to eliminate overhead when accessing current configuration.
///
class SensorConfiguration : public BasicConfigurator {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Static 'macro' hardcoding the maximum number of sensors on a Trigno system.
    ///
    // static constexpr uint NSENSORS = 16;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      id       ID of the sensor to configure, as a sensor::ID value.
    /// @param      network  Network handler/interface. Must provide InterfaceType::query() members to allow bidirectional message exchange.
    ///
    /// @throw      std::invalid_argument if id falls outside expected range (number of pairing positions on Trigno base)
    ///
    explicit SensorConfiguration(sensor::ID id, Interface* network);

    //--------------------------------------------------------------------------
    /// @brief      Resets configuration to default values.
    ///
    void reset() override;

    //--------------------------------------------------------------------------
    /// @brief      Get configuration from Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    bool get() override;

    //--------------------------------------------------------------------------
    /// @brief      Set hardware configuration into Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    bool set() override;

    //--------------------------------------------------------------------------
    /// @brief      Custom conversion operator to text string.
    ///
    /// @note       Useful to collapse all configuration into a single string that can be sent to the SDK server for bulk processing.
    ///             Can also be used to get visual feedback of configuration content.
    ///
    operator std::string() const override;

    //--------------------------------------------------------------------------
    /// @brief      Sensor ID. Non-configurable.
    ///
    sensor::ID id() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Sensor serial number.
    ///
    const std::string& serial() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Sensor firmware version.
    ///
    const std::string& firmware() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Access sensor type.
    ///
    /// @return     Sensor type/family alphanumeric identifier.
    ///
    /// @note       Sensor types and their identifier characters are listed in the <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
    ///
    char type() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Access current operating mode of the sensor.
    ///
    /// @return     Mode numeric identifier.
    ///
    /// @note       Sensor operating modes are extensively listed in the <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
    ///
    /// @note       There is a significant number of possible operating modes for each Trigno sensor. Due to this, it was opted to *not* take into account when developing trignoclient,
    ///             as it would clutter the source and add little in terms of performance or usability. It is therefore up to the user to select a valid mode for the sensor type, and delegate the
    ///             validity check to the Trigno server by parsing query response;
    ///
    size_t mode() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Total number of data channels (EMG + AUX) available on the sensor current operating mode.
    ///
    size_t nChannels() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Number of EMG channels available on the sensor current operating mode.
    ///
    size_t nEMGChannels() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Number of AUX channels available on the sensor current operating mode.
    ///
    size_t nAUXChannels() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Position of first data value in the data frame.
    ///
    size_t startIndex() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Number of native samples (?) per frame for each channel
    ///
    const std::vector< size_t >& samplesPerFrame() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Native sample rate for each channel (EMG or AUX)
    ///
    const std::vector< float >& sampleRate() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Gain for each channel
    ///
    const std::vector< float >& gain() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Units used for each channel
    ///
    const std::vector< std::string >& units() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Sensor current range. Limited configuration (mode dependent).
    ///
    /// @return     True if range is set to LOW (+/-5.5mV), false if range is set to HIGH (+/-11mV).
    ///
    bool lowRange() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Sensor currrent bandwitdh. Limited configuration (mode dependent).
    ///
    /// @return     True if bandwith is set to NARROW (20-450Hz), false if bandwith is set to WIDE (10-850Hz).
    ///
    bool narrowBandwidth() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Check if sensor is paired.
    ///
    /// @return     True if paired, False otherwise.
    ///
    bool isPaired() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Check if sensor is active.
    ///
    /// @return     True if active, False otherwise.
    ///
    bool isActive() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Issue a pair request to the Trigno server, which requires user intervention (button press, NFC/RFID pass).
    ///
    /// @return     True if pairing was sucessful, False otherwise.
    ///
    /// @note       Not very robust, requires testing. At this stage it is recommended to pair desired sensors prior to using the SDK.
    ///
    bool pair();

    //--------------------------------------------------------------------------
    /// @brief      Set sensor mode.
    ///
    /// @param[in]  mode  Sensor mode (numerical identifier). Cf. <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
    ///
    /// @return     True if mode was sucessfully set, False otherwise.
    ///
    bool setMode(size_t mode);

    //--------------------------------------------------------------------------
    /// @brief      Set sensor range.
    ///
    /// @param[in]  low   Low range (+/-5.5mV) flag. If false, range is set to High (+/-11mV).
    ///
    /// @return     True if range was sucessfully set, False otherwise.
    ///
    bool setRange(bool low);

    //--------------------------------------------------------------------------
    /// @brief      Set sensor bandwith.
    ///
    /// @param[in]  narrow   Narrow bandwidth (20-450Hz) flag. If false, bandwidth is set to Wide (10-850Hz).
    ///
    /// @return     True if bandwith was sucessfully set, False otherwise.
    ///
    bool setBandwidth(bool narrow);

    //--------------------------------------------------------------------------
    /// @brief      Forward declaration of MultiSensorConfiguration as friend class, in order to provide access to protected members.
    ///
    friend class MultiSensorConfiguration;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Sensor ID.
    ///
    sensor::ID _id;

    //--------------------------------------------------------------------------
    /// @brief      Pair status.
    ///
    bool _paired;

    //--------------------------------------------------------------------------
    /// @brief      Active status.
    ///
    bool _active;

    //--------------------------------------------------------------------------
    /// @brief      Sensor type specifier (alphanumeric character).
    ///
    /// @note       Sensor types and their identifiers are listed in the <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
    ///
    char _type;

    //--------------------------------------------------------------------------
    /// @brief      Sensor operating mode.
    ///
    /// @note       Operating modes available for each sensor type are listed in the <a href="https://www.delsys.com/downloads/USERSGUIDE/trigno/sdk.pdf">Trigno Wireless System SDK</a> documentation.
    ///
    size_t _mode;

    //--------------------------------------------------------------------------
    /// @brief      Number of active data channels on sensor (type- & mode-dependent).
    ///
    size_t _n_channels;

    //--------------------------------------------------------------------------
    /// @brief      Number of active EMG channels on sensor (type- & mode-dependent).
    ///
    size_t _n_emg_channels;

    //--------------------------------------------------------------------------
    /// @brief      Number of active AUX (e.g. IMU, FSR, etc) channels on sensor (type- & mode-dependent).
    ///
    size_t _n_aux_channels;

    //--------------------------------------------------------------------------
    /// @brief      Position of first data value specific to this sensor in the system sample/frame.
    ///
    size_t _start_index;

    //--------------------------------------------------------------------------
    /// @brief      Sensor firmware version (as text)
    ///
    /// @note       Non-configurable (physical property).
    ///
    std::string _firmware;

    //--------------------------------------------------------------------------
    /// @brief      Sensor firmware version (as text)
    ///
    /// @note       Non-configurable (physical property).
    ///
    std::string _serial;

    //--------------------------------------------------------------------------
    /// @brief      Number of native samples (?) per frame for each channel
    ///
    std::vector< size_t > _n_samples_frame;

    //--------------------------------------------------------------------------
    /// @brief      Native sample rate for each channel (EMG or AUX)
    ///
    std::vector< float > _sample_rate;

    //--------------------------------------------------------------------------
    /// @brief      Gain for each channel
    ///
    std::vector< float > _gain;

    //--------------------------------------------------------------------------
    /// @brief      Units used for each channel
    ///
    std::vector< std::string > _units;

    //--------------------------------------------------------------------------
    /// @brief      Low range flag.
    ///
    bool _low_range;

    //--------------------------------------------------------------------------
    /// @brief      Narrow bandwidth flag.
    ///
    bool _narrow_bandwidth;
};



//------------------------------------------------------------------------------
/// @brief      Class describing a set of SensorConfiguration instances, for global handling of a Trigno System.
///             Provides individual sensor labelling functionality, resulting in a cleaner interface.
///
class MultiSensorConfiguration : public std::index< SensorConfiguration, std::string /* sensor label type */, true /* locked */ > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      network  Network handler/interface. Must provide InterfaceType::query() members to allow bidirectional message exchange.
    ///
    explicit MultiSensorConfiguration(Interface* network);

    //--------------------------------------------------------------------------
    /// @brief      Get configuration from Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    bool get();

    //--------------------------------------------------------------------------
    /// @brief      Get a list of paired sensors.
    ///
    sensor::List getPaired() const;

    //--------------------------------------------------------------------------
    /// @brief      Get a list of active sensors.
    ///
    sensor::List getActive() const;

    //--------------------------------------------------------------------------
    /// @brief      Gets sensor index (position in data frames) from its ID.
    ///
    /// @param[in]  id    Sensor identifier (as sensor::ID value).
    ///
    /// @return     Zero-based index corresponding to the expected position of the sensor in full data frames.
    ///
    /// @note       Unless all sensors are active and/or data frames have data for all sensors, IDs do not match position in Frames.
    ///
    size_t index(sensor::ID id);

    //--------------------------------------------------------------------------
    /// @brief      Access sensor label.
    ///
    /// @param[in]  id    Sensor identifier (as sensor::ID value).
    ///
    /// @return     Reference to element descriptor on the underlaying container.
    ///
    std::string& label(sensor::ID id) noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Access sensor label (const overload).
    ///
    /// @param[in]  id    Sensor identifier (as sensor::ID value).
    ///
    /// @return     Reference to element descriptor on the underlaying container.
    ///
    const std::string& label(sensor::ID id) const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Creates a structured (empty) frame from current configuration (preallocates container for active sensors/channels).
    ///
    // trigno::DataFrame createFrame(float timestamp = 0.0) const noexcept;
};



//------------------------------------------------------------------------------
/// @brief      Class that describes the properties of a Trigno base station (where sensors are paired to).
///
class BaseInformation : public BasicConfigurator {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      network  Network handler/interface. Must provide InterfaceType::query() members to allow bidirectional message exchange.
    ///
    explicit BaseInformation(Interface* network);

    //--------------------------------------------------------------------------
    /// @brief      Resets configuration to default values.
    ///
    void reset() override;

    bool get() override;

    bool set() override;

    operator std::string() const override;

    const std::string& serial() const noexcept;

    const std::string& firmware() const noexcept;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Sensor firmware version (as text).
    ///
    std::string _firmware;

    //--------------------------------------------------------------------------
    /// @brief      Sensor serial number (as text).
    ///
    std::string _serial;
};



//------------------------------------------------------------------------------
/// @brief      Top-level controller class for the Trigno system, handling start/stop calls and trigger configuration.
///
class SystemControl : public BasicConfigurator {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Maximum number of paired/active sensors on the Trigno system.
    ///
    static constexpr uint NUMBER_OF_SENSORS = 16;

    //--------------------------------------------------------------------------
    /// @brief      Trigger configuration descriptor.
    ///
    struct Trigger {
        bool start;
        bool stop;
    };

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      network  Network handler/interface. Must provide InterfaceType::query() members to allow bidirectional message exchange.
    ///
    explicit SystemControl(Interface* network);

    //--------------------------------------------------------------------------
    /// @brief      Resets configuration to default values.
    ///
    void reset() override;

    bool get() override;

    bool set() override;

    operator std::string() const override;

    bool isRunning() const;

    const Trigger& trigger() const;

    bool start(float delay = 0.0);

    bool stop(float delay = 0.0);

    bool quit();


 protected:
    //--------------------------------------------------------------------------
    /// @brief      Sensor firmware version (as text)
    ///
    bool _running;

    //--------------------------------------------------------------------------
    /// @brief      Start/Stop trigger configuration.
    ///
    Trigger _trigger;
};



//------------------------------------------------------------------------------
/// @brief      Class that describes configurable network parameters of a Trigno Research+ EMG system.
///
/// @note       Intended to describe current configuration of a remote server, thus several parameters can't be freely modified.
///             Can't be user-instantiated, in order to keep parameters within valid ranges at all times.
///
/// @note       Configuration container only - agnostic to how data is fetched from a remote server.
///             Simplifies implementation and delegates construction to trigno::network::Interface friend class.
///
/// @note       Most common use case has each instance tied to a network::Client establishing the bridge to the Trigno system. Alternatively, can be used
///             unattached to any Client instance e.g. in local IO operations or when defining target configuration to be later set on a connected system.
///
class ConnectionConfiguration : public BasicConfigurator {
 public:
    //--------------------------------------------------------------------------
    /// @brief      TCP port used for seding/receiving queries & commands to the Trigno server.
    ///
    static constexpr uint COMMAND_PORT = 50040;

    //--------------------------------------------------------------------------
    /// @brief      TCP port used for receiving EMG data.
    ///
    static constexpr uint EMG_DATA_PORT = 50043;

    //--------------------------------------------------------------------------
    /// @brief      TCP port used for receiving AUX data.
    ///
    static constexpr uint AUX_DATA_PORT = 50044;

    //--------------------------------------------------------------------------
    /// @brief      Number of EMG data channels per sensor
    ///
    static constexpr uint EMG_DATA_CHANNELS_PER_SENSOR = 1;

    //--------------------------------------------------------------------------
    /// @brief      Number of AUX data channels per sensor
    ///
    static constexpr uint AUX_DATA_CHANNELS_PER_SENSOR = 9;

    //--------------------------------------------------------------------------
    /// @brief      TCP port used for receiving EMG data (legacy).
    ///
    static constexpr uint EMG_DATA_CHANNEL_LEGACY = 50041;

    //--------------------------------------------------------------------------
    /// @brief      TCP port used for receiving AUX data (legacy).
    ///
    static constexpr uint AUX_DATA_CHANNEL_LEGACY = 50042;

    //--------------------------------------------------------------------------
    /// @brief      Timeout value (seconds) when establishing TCP connections with command & data channels.
    ///
    /// @todo       Declare as dynamic parameter?
    ///
    static constexpr int CONNECT_TIMEOUT = 1000 /* milliseconds */;

    //--------------------------------------------------------------------------
    /// @brief      Timeout value (seconds) when reading/writing from/to TCP sockets.
    ///
    /// @todo       Declare as dynamic parameter?
    ///
    static constexpr int IO_TIMEOUT = 10 /* milliseconds */;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      network  Network handler/interface. Must provide InterfaceType::query() members to allow bidirectional message exchange.
    ///
    explicit ConnectionConfiguration(Interface* network);

    //--------------------------------------------------------------------------
    /// @brief      Resets configuration to default values.
    ///
    void reset() override;

    //--------------------------------------------------------------------------
    /// @brief      Get configuration from Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    bool get() override;

    //--------------------------------------------------------------------------
    /// @brief      Set hardware configuration into Trigno Server.
    ///
    /// @return     True if parameters were successfully received and parsed, false otherwise.
    ///
    bool set() override;

    //--------------------------------------------------------------------------
    /// @brief      Custom conversion operator to text string.
    ///
    /// @note       Useful to collapse all configuration into a single string that can be sent to the SDK server for bulk processing.
    ///             Can also be used to get visual feedback of configuration content.
    ///
    operator std::string() const override;

    //--------------------------------------------------------------------------
    /// @brief      Trigno SDK Server Protocol version (non-configurable).
    ///
    /// @note       TCP server returns the version on each new connection, thus version should be available without any server query.
    ///
    const std::string& version() const;

    //--------------------------------------------------------------------------
    /// @brief      Master/slave flag. By convention, first connect is set as master while subsequent connections are slaves.
    ///             If set to true, connection is assigned to master by the Trigno server.
    ///
    /// @note       Reference to internal member. Parameter can't be configured, thus can't be changed.
    ///
    bool isMaster() const;

    //--------------------------------------------------------------------------
    /// @brief      Backward compatibility setting.
    ///
    /// @note       When disabled, the sample rates for the EMG Data ports will scale up to the highest configured EMG sample rate, and the sample rates for the AUX Data
    ///             ports will scale up to the highest configured auxiliary rate. Alternatively, when enabled, the sample rates are locked to either 2000/1925.925/1111.111 Hz (EMG)
    ///             and 148.148 Hz (AUX).
    ///
    bool backwardsCompatibility() const;

    //--------------------------------------------------------------------------
    /// @brief      Access backward compatibility setting (const overload).
    ///
    /// @note       Only valid if backwards compatiblity is enabled. When enabled, all high frequency channels will be sampled at 2000Hz. Otherwise EMG channels will be sampled at
    ///             1925.925 Hz (EMG Data) or 1111.111 Hz (IM EMG Data) and all AUX Data ports will be sampled at 148.148 Hz.
    ///
    bool upsampling() const;

    //--------------------------------------------------------------------------
    /// @brief      Endianess flag. If true, data is streamed in big endian format, otherwise little endian format is used.
    ///
    bool bigEndian() const;

    //--------------------------------------------------------------------------
    /// @brief      Maximum number of samples per frame for the EMG data channels.
    ///
    size_t maxEMGSamples() const;

    //--------------------------------------------------------------------------
    /// @brief      Maximum number of samples per frame for the AUX data channels.
    ///
    size_t maxAUXSamples() const;

    //--------------------------------------------------------------------------
    /// @brief      Time interval between consecutive frames (non-configurable).
    ///
    float frameInterval() const;

    //--------------------------------------------------------------------------
    /// @brief      Sets TCP connection as master (for starting/stoping data streaming).
    ///
    /// @return     True if connection was successfully set as master, false otherwise.
    ///
    bool setMaster();

    //--------------------------------------------------------------------------
    /// @brief      Sets the backwards compatibility flag.
    ///
    /// @param[in]  backwards_compability  Whether backwards compatibility is desired. Defaults to true.
    ///
    /// @return     True if backwards compatibility was successfully changed, false otherwise.
    ///
    bool setBackwardsCompability(bool backwards_compability = true);

    //--------------------------------------------------------------------------
    /// @brief      Sets the upsampling flag.
    ///
    /// @param[in]  upsampling  Whether upsampling is desired. Defaults to true.
    ///
    /// @return     True if upsampling was successfully changed, false otherwise.
    ///
    bool setUpsampling(bool upsampling = true);

    //--------------------------------------------------------------------------
    /// @brief      Sets data endianess.
    ///
    /// @param[in]  big_endian  Whether big endian is desired. Defaults to true.
    ///
    /// @return     True if endianess was successfully changed, false otherwise.
    ///
    bool setBigEndian(bool big_endian = true);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Trigno SDK version. Fixed, can't be configured.
    ///
    std::string _version;

    //--------------------------------------------------------------------------
    /// @brief      Master connection flag.
    ///
    bool _master;

    //--------------------------------------------------------------------------
    /// @brief      Backwards compatibility flag.
    ///
    bool _backwards_compatibility;

    //--------------------------------------------------------------------------
    /// @brief      Upsampling flag.
    ///
    bool _upsampling;

    //--------------------------------------------------------------------------
    /// @brief      Big endian flag.
    ///
    bool _big_endian;

    //--------------------------------------------------------------------------
    /// @brief      Max EMG samples per frame.
    ///
    size_t _max_emg_samples;

    //--------------------------------------------------------------------------
    /// @brief      Max AUX samples per frame.
    ///
    size_t _max_aux_samples;

    //--------------------------------------------------------------------------
    /// @brief      Interval between frames sent over the network. Fixed-value, can't be configured.
    ///
    float _frame_interval;
};

}  // namespace trigno::network



//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  emg_frame  Sensor configuration instance.
///
/// @return     Modified data stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::network::SensorConfiguration& sensor_configuration);



//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  emg_frame  Base information instance.
///
/// @return     Modified data stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::network::BaseInformation& base_information);



//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  emg_frame  System control instance.
///
/// @return     Modified data stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::network::SystemControl& system_control);



//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  emg_frame  Connection configuration instance.
///
/// @return     Modified data stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::network::ConnectionConfiguration& connection_configuration);


#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_CONFIGURATION_HPP_
