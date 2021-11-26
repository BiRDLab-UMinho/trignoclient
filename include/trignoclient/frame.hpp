//------------------------------------------------------------------------------
/// @file       data_frame.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of trigo::Frame class, a container to hold measurements from a Trigno Research+ system at a specific time.
///
/// Frame represents a full-system data sample at a specific time, holding samples for each active sensor (as a std::index< trigno::Sample > instance).
/// Indexing functionality provided by std::index<> ensures an intuitive container interface for dealing with different sensors.
///
/// Additionaly, a nested type Frame::Stamped represents the time-stamped version of Frame, making use of std::tagged<>, which is used
/// by DataSequence to sort between consecutive frames.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DATAFRAME_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DATAFRAME_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include "std/indexer.hpp"    // std::index
#include "std/not_implemented.hpp" // std::not_implemented
#include "sensor.hpp"         // trigno::sensor::ID
#include "sample.hpp"         // trigno::Sample, trigno::Sample::Ptr
#include "configuration.hpp"  // trigno::network::MultiSensorConfiguration

namespace trigno {

//------------------------------------------------------------------------------
/// @brief      Class describing a complete multi-sensor EMG data frame from a Trigno system.
///             Aggregates EMG value for multiple sensors.
///
/// @note       Individual sensor labels/descriptors are handled by base class std::index< Sample >.
///
/// @note       Number of sensors are not fixed, despite it being a fixed value on a Trigno system.
///             This allows Frame to be flexible and only include data for some/all sensors (e.g. only active).
///
class Frame : public std::index< Sample > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Pointer type.
    ///
    using Ptr = std::shared_ptr< Frame >;

    //--------------------------------------------------------------------------
    /// @brief      Type used to represent the timestamp of the frame.
    ///
    using TimeStamp = float;

    //--------------------------------------------------------------------------
    /// @brief      Stamped type, for synchronization/sequencing purposes.
    ///             Represents a portable/movable type for performance purposes.
    ///
    /// @note       Assigns a single time step to all sensor data. Trigno systems sample same-type (EMG and AUX) channels @ same frequency.
    ///
    /// @note       Employ std::tagged<> class in a generic approach, using a floatint point key to identify Frame.
    ///             This is useful when aggregating multiple frames in a sequence, allowing the use of std::series<> indexing/sorting implementation.
    ///
    /// @todo       Consider using std::unique_ptr<> to avoid having same Frame with multiple timestamps
    ///
    using Stamped = std::tagged< Frame, TimeStamp >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (empty) instance, from number of sensors/channels.
    ///
    /// @param[in]  n_sensors   Number of sensors. Sensor samples are initialized to single-channel, null values.
    /// @param[in]  n_channels  Number of channels per sensor (applied to all sensors). Defaults to 1 i.e. single channel sensors/
    ///                         In case of different types of sensors (or sensors in different operating modes) in the same system, it is recommended to use the configuration constructor overload.
    ///
    explicit Frame(size_t n_sensors = 0, size_t n_channels = 1);


    Frame(const Frame& other) = default;  // { printf("cop1\n"); }
    Frame(Frame&& other) : std::index< Sample >() { _data = std::move(other._data); }  // = default;  // { printf("mov3\n"); }

    Frame& operator=(const Frame& other) = default;  // { printf("cop1 assn\n"); return *this; }
    Frame& operator=(Frame&& other) = default;  // { _data = std::move(other._data); return *this; } //  { printf("mov3 assn\n"); return *this; }

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance, from a container of sensor samples (a.k.a. Sample).
    ///
    /// @param[in]  source      Generic container with Sample instances. *Must* be iteratable (STL-like).
    /// @param[in]  labels      Container (as sensor::Labels) with labels for each sensor in frame.
    ///                         Dimensions must matche *source*.
    ///
    /// @tparam     C           Input container type. Defaults to std::vector< Sample >.
    ///
    /// @note       Implements a pseudo-copy constructor from other STL-like containers. Base class already provides copy constructors.
    ///
    template < typename C = std::vector< Sample > >
    explicit Frame(const C& source, const sensor::Labels& labels);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance from a simple array of Sample::Value objects and a header string (with sensor/channel labels).
    ///
    /// @param[in]  source  Generic container with Sample::Value instances. *Must* be iteratable (STL-like).
    /// @param[in]  header  Header/descriptor string with sensor-channel labels.
    ///                     Sensor channels should be labelled with same prefix and " C#N" suffix.
    ///
    /// @tparam     C       Input container type. Defaults to std::vector< Sample >.
    ///
    template < typename C = std::vector< Sample::Value > >
    explicit Frame(const C& source, const std::string& header);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance with samples for *sensors* from given *frame*.
    ///
    /// @param[in]  frame    Input data frame.
    /// @param[in]  sensors  List of sensor IDs.
    ///
    explicit Frame(const Frame& frame, const sensor::List& sensor_ids);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance with samples for *sensors* from given *frame*.
    ///
    /// @param[in]  frame    Input data frame.
    /// @param[in]  sensors  List of sensor labels.
    ///
    explicit Frame(const Frame& frame, const sensor::Labels& sensor_labels);

    //--------------------------------------------------------------------------
    /// @brief      Get list of sensor IDs in frame samples.
    ///
    /// @return     Instance of sensor::List
    ///
    /// @note       Useful when checking if frame contains data for specific sensors or when comparing different frames
    ///             e.g. frame.sensors() == other.sensors().
    ///
    sensor::List sensors() const;

    //--------------------------------------------------------------------------
    /// @brief      Get the dimension (total number of channels) if the frame.
    ///
    /// @note       Not to be confused with base method size() (i.e. number of sensor samples).
    ///
    /// @note       Channels counted @ each call -> O(n) complexity.
    ///
    size_t dim() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Resets data without restructuring frame i.e. sets all values to given *values*.
    ///
    /// @param[in]  values Container with sample values (as std::vector< Sample::Value >). Size must either be unitary or match frame dimension.
    ///
    /// @throw      std::invalid_argument if input values != 1 *or* != dim().
    ///
    void reset(const std::vector< Sample::Value >& values);

    //--------------------------------------------------------------------------
    /// @brief      Resets data without restructuring frame i.e. sets all values to a single given *value*.
    ///
    /// @param[in]  value  Value to assign to frame.
    ///
    void reset(Sample::Value value = 0.0);

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of find() base member to avoid shadowing by derived overloads.
    ///
    using std::index< Sample >::find;

    //--------------------------------------------------------------------------
    /// @brief      Searches for sample matching given sensor *id*.
    ///
    /// @param[in]  id    Sensor identifier.
    ///
    /// @return     Position/index of sensor if found, -1 otherwise.
    ///
    int find(sensor::ID id) const;

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of operator[]() base member to avoid shadowing by derived overloads.
    ///
    using std::index< Sample >::operator[];

    //--------------------------------------------------------------------------
    /// @brief      Construct a new data frame with samples for the specified *sensors*.
    ///             Requested sensors without data on frame are ignored.
    ///
    /// @param[in]  sensors  List of sensor IDs.
    ///
    /// @return     Data frame.
    ///
    /// @note       Useful with rvalue reference/instances of Frame, as it allows the compiler to bypass a temporary e.g. 'my_frame = client.read()[ID_1, ID_7]''
    ///
    /// @todo       Use a parameter pack instead of initializer list, discarding the use of {}.
    ///
    Frame operator[](const sensor::List& sensor_ids) const;

    //--------------------------------------------------------------------------
    /// @brief      Construct a new data frame with samples for the specified *sensors*.
    ///             Requested sensors without data on frame are ignored.
    ///
    /// @param[in]  sensors  List of sensor labels.
    ///
    /// @return     Data frame.
    ///
    /// @note       Useful with rvalue reference/instances of Frame, as it allows the compiler to bypass a temporary e.g. 'my_frame = (*client.read().value)[{ ID_1, ID_7 }]''
    ///
    /// @todo       Use a parameter pack instead of initializer list, discarding the use of {}.
    ///
    Frame operator[](const sensor::Labels& sensor_labels) const;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a text string with inidividual sensor identifiers (labels).
    ///
    /// @param[in]  emg_frame  Data frame with EMG data.
    /// @param[in]  delimiter  Delimiter character. Defaults to comma (',').
    ///
    /// @return     Instance of std::string with sensor labels separated by *delimiter*.
    ///
    template < typename T = Frame::Stamped >
    static std::string header(const T& frame, const std::string& delimiter = ",");

    //--------------------------------------------------------------------------
    /// @brief      Constructs a text string with inidividual sensor values/readings.
    ///
    /// @param[in]  emg_frame  Data frame with EMG data.
    /// @param[in]  delimiter  Delimiter character. Defaults to comma (',').
    ///
    /// @return     Instance of std::string with sensor data separated by *delimiter*.
    ///
    template < typename T = Frame::Stamped >
    static std::string to_string(const T& frame, const std::string& delimiter = ",");

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (empty) frame for EMG data, from system configuration.
    ///
    /// @param[in]  configuration  System configuration, which is used to structure the Frame (number of sensors & channels).
    ///
    static Frame EMG(const network::MultiSensorConfiguration& configuration);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (empty) frame for AUX data, from system configuration.
    ///
    /// @param[in]  configuration  System configuration, which is used to structure the Frame (number of sensors & channels).
    ///
    static Frame AUX(const network::MultiSensorConfiguration& configuration);

    //--------------------------------------------------------------------------
    /// @brief      Static factory constructor.
    ///
    /// @param[in]  args  Constructor arguments.
    ///
    /// @tparam     Args  Variadic parameter pack (implicitely deduced) describing type list of constructor arguments.
    ///
    /// @return     Shareable smart pointer instance (a.k.a. Frame::Ptr).
    ///
    template < typename... Args >
    static Ptr create(Args... args) { return Ptr(new Frame(std::forward< Args >(args)...)); }

    //--------------------------------------------------------------------------
    /// @brief      Static factory constructor (overload for time-stamped data frames).
    ///
    /// @param[in]  stamp  Frame time stamp
    /// @param[in]  args   Constructor arguments.
    ///
    /// @tparam     Args   Variadic template parameter pack i.e. argumetn types
    ///
    /// @return     Stamped instance (a.k.a. std::tagged< Frame, Frame::TimeStamp >)
    ///
    /// @note       Stamp an existing data frame, by moving it into Stamped object.
    ///
    template < typename... Args >
    static Stamped create(TimeStamp stamp, Args... args) { return Stamped(stamp, std::forward< Args >(args)...); }

    //--------------------------------------------------------------------------
    /// @brief      Initializes an empty Frame instance a frame header string.
    ///
    /// @param[in]  header     Header/descriptor string with sensor-channel labels.
    ///                        Sensor channels should be labelled with same prefix and " C#N" suffix.
    /// @param[in]  delimiter  Delimiter character. Defaults to comma (',').
    ///
    /// @tparam     T          Output type. One of Frame or Frame::Stamped (default).
    ///
    template < typename T = Frame::Stamped >
    static T initialize(const std::string& header, const std::string& delimiter = ",");

    //--------------------------------------------------------------------------
    /// @brief      Loads a frame from serialized content i.e. text string. Useful to parse from IO streams and/or file input.
    ///             More versatile than left shift operator ()
    ///
    /// @param[in]  data       Input text w/ frame content (*delimiter*-separated values of *Sample::Value*).
    /// @param[in]  sensors    Sensor labels. Used to attribute headers to output frame.
    /// @param[in]  delimiter  Delimiter character. Defaults to comma (',').
    ///
    /// @tparam     T          Output type. One of Frame or Frame::Stamped (default).
    ///
    /// @throws     std::runtime_error if *input* has not enough values to populate *frame* (!= dim()).
    ///
    template < typename T = Frame::Stamped >
    static void load(T& frame, const std::string& input, const std::string& delimiter = ",");

    //--------------------------------------------------------------------------
    /// @brief      Loads a frame from a local file. Wraps around initialize() and load() overload.
    ///
    /// @param[in]  path       Path to local file with frame content.
    /// @param[in]  index      Frame index i.e. which frame in *path* to load, in cases where multiple files are available. Defaults to 0.
    /// @param[in]  header     Header to use. If empty (default), first line is used to label sensor/channels of output frame.
    /// @param[in]  delimiter  Delimiter character. Defaults to comma (',').
    ///
    /// @tparam     T          Output type. One of Frame or Frame::Stamped (default).
    ///
    /// @throws     std::runtime_error if *path* has no or invalid data @ *index*.
    ///
    template < typename T = Frame::Stamped >
    T load(const std::string& path, size_t index = 0, const std::string& header = "", const std::string& delimiter = ",");


    // template < typename T = Frame::Stamped >
    // static bool write(const T& frame, const std::string& path);
};



template < typename C >
Frame::Frame(const C& source, const sensor::Labels& labels) {
    reserve(source.size());
    size_t idx = 0;
    for (const auto& sample : source) {
        if (!labels.empty()) {
            emplace_back(labels[idx++], source);
        } else {
            emplace_back("S#" + std::to_string(idx++), source);
        }
    }
}



template < typename C >
Frame::Frame(const C& source, const std::string& header) {
    /* ... */
    throw std::not_implemented(__func__);
    /* ... */
}

}  // namespace trigno


//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  emg_frame  Frame instance.
///
/// @return     Modified output stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::Frame& frame);



//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  emg_frame  Stamped Frame instance.
///
/// @return     Modified output stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::Frame::Stamped& frame);



//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for input stream operations.
///
/// @param      ostream    The stream to get data from.
/// @param[in]  emg_frame  Frame instance.
///
/// @return     Modified input stream.
///
/// @note       *frame* must be properly initialize to fit data on input stream.
///             If file stream, be sure to call initialize() from local path.
///
std::istream& operator>>(std::istream& istream, trigno::Frame& frame);



//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for input stream operations.
///
/// @param      ostream    The stream to get data from.
/// @param[in]  emg_frame  StampedFrame instance.
///
/// @return     Modified input stream.
///
/// @note       *frame* must be properly initialize to fit data on input stream.
///             If file stream, be sure to call initialize() from local path.
///
std::istream& operator>>(std::istream& istream, trigno::Frame::Stamped& frame);

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DATAFRAME_HPP_
