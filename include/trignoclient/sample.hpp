//------------------------------------------------------------------------------
/// @file       data_frame.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of trigo::Sample class, a simple container to hold sample data for a single Trigno sensor.
///
/// trigno::Sample represents the most elementar data container / representation used in the *trignoclient* library, forming a bases of Frame and Sequence classes.
/// Wraps around a std::vector in order to dynamically account for single & multichannel configurations, while exposing only limited container functionalities for simplicity.
/// Can be iterated, random-accessed and implicitly converted to a single value type.
///
/// Overhead caused by using std::vector should be negligible in most practical applications due to its small dimensions, even considering high sampling rates of Trigno systems.
/// In truth, using std::tagged<> to index and pass instances of trigno::Sample should eliminate any potential overhead.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SENSORSAMPLE_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SENSORSAMPLE_HPP_

#include <vector>
#include <iostream>
#include <memory>     // std::shared_ptr<>
#include <utility>    // strd::forward<>
#include "sensor.hpp" // trigno::sensor::ID

namespace trigno {

//------------------------------------------------------------------------------
/// @brief      Class describing data for a single sensor with a variable number of channels.
///
/// @note       Wraps around std::vector< float >, but provides a read-only interface & is implicitly convertible to float,
///             which is useful in cases where there is only a single active EMG data channel.
///
class Sample {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Pointer type.
    ///
    using Ptr = std::shared_ptr< Sample >;

    //--------------------------------------------------------------------------
    /// @brief      Sample value type.
    ///
    using Value = float;

    //--------------------------------------------------------------------------
    /// @brief      Type of the underlying container.
    ///
    using Container = std::vector< Value >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance with given size, while (optionally) initializing values from raw array.
    ///
    /// @param[in]  n_channels  Number of channels.
    /// @param      raw_data    Raw byte array with channel data. Defaults to nullptr i.e. channels are initialized to null values.
    ///
    explicit Sample(sensor::ID id, size_t n_channels, const char* raw_data = nullptr);

    Sample(const Sample& other) = default;  // { printf("sample copy!\n"); }
    Sample(Sample&& other) = default;  // { printf("sample move!\n"); }

    Sample& operator=(const Sample& other) = default;  // { printf("sample copy assn!\n"); }
    Sample& operator=(Sample&& other) = default;  // { printf("sample move assn!\n"); }

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance, from given channel values.
    ///
    /// @param[in]  data  Instance of type *Container* w/ data values (for each channel).
    ///                   Bracket-enclosed list can be used (implicitly convertible to *Container*).
    ///                   Defaults to empty list i.e. no values (pseu-default constructor).
    ///
    explicit Sample(sensor::ID id, const Container& values = { /* ... */ });

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~Sample() = default;

    //--------------------------------------------------------------------------
    /// @brief      Access ID of sensor associated with sample
    ///
    /// @note       Kept for redundancy - Sample is not intended to be explicitely instantiated but rather indexed in data container/structures, attached to sensor label.
    ///
    sensor::ID id() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Checks if sample is empty (has data).
    ///
    /// @return     True if sample has values (size > 0), false otherwise.
    ///
    bool empty() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Returns the size of the sample == number of data channels.
    ///
    size_t size() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Access EMG channel values.
    ///
    /// @param[in]  channel  Channel index.
    ///
    /// @return     Reference to value @ *channel* position.
    ///
    /// @throw      std::out_of_range if channel is not within valid range.
    ///
    Value& operator[](size_t channel);

    //--------------------------------------------------------------------------
    /// @brief      Access EMG channel values (const overload).
    ///
    /// @param[in]  channel  Channel index.
    ///
    /// @return     Reference to value @ *channel* position.
    ///
    /// @throw      std::out_of_range if channel is not within valid range.
    ///
    const Value& operator[](size_t channel) const;

    //--------------------------------------------------------------------------
    /// @brief      Conversion operator to floating point value. Returns the value of the first EMG channel.
    ///
    /// @note       Useful when only 1 channel has data, discards the need to use [0] subscript.
    ///
    operator const Value&() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Computes the average between available channels;
    ///
    /// @note       Useful in multi-channel frames.
    ///
    Value average() const;

    //--------------------------------------------------------------------------
    /// @brief      Get an iterator for the underlying container (@ front, non-const).
    ///
    Container::iterator begin();

    //--------------------------------------------------------------------------
    /// @brief      Get an iterator for the underlying container (@ front, const).
    ///
    Container::const_iterator begin() const;

    //--------------------------------------------------------------------------
    /// @brief      Get an iterator for the underlying container (@ end, non-const).
    ///
    Container::iterator end();

    //--------------------------------------------------------------------------
    /// @brief      Get an iterator for the underlying container (@ end, const).
    ///
    Container::const_iterator end() const;

    //--------------------------------------------------------------------------
    /// @brief      Static factory constructor.
    ///
    /// @param[in]  args  Constructor arguments.
    ///
    /// @tparam     Args  Variadic parameter pack (implicitely deduced) describing type list of constructor arguments.
    ///
    /// @return     Shareable smart pointer instance (a.k.a. Sample::Ptr).
    ///
    template < typename... Args >
    static Ptr create(Args... args) { return Ptr(new Sample(std::forward< Args >(args)...)); }

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Sensor/source identifier.
    ///
    sensor::ID _id;

    //--------------------------------------------------------------------------
    /// @brief      Data container.
    ///
    Container _data;
};

}  // namespace trigno

//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for output stream operations.
///
/// @param      ostream    The stream to write data to.
/// @param[in]  sample     Sample instance.
///
/// @return     Modified data stream.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::Sample& sample);

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SENSORSAMPLE_HPP_
