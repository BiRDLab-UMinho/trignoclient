//------------------------------------------------------------------------------
/// @file       sequence.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of trigo::Sequence class, a container to a sequence of time-indexed Frame (a.k.a. StampedFrame) instances e.g. as acquired with a Trigno system
///             or as result of processing operations.
///
/// Sequence derives from std::series<> to facilitate handling (inserting/removing) multiple dataframes in time sequence.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SEQUENCE_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SEQUENCE_HPP_

#include <deque>
#include "std/sorted_indexer.hpp"  // std::series<>
#include "std/range_iterator.hpp"  // std::range_iterator<>
#include "frame.hpp"               // trigno::Frame

namespace trigno {

//------------------------------------------------------------------------------
/// @brief      Generic data sequence/trajectory container wrapper that hold multiple consecutive time-indexed data frames.
///
/// @note       std::series<> provides base container implementation (cf. std/indexer.hpp). It employs a std::deque<> as the underlying container type, and is specialized
///             to large number of sorted values (such as high frequency time-indexed values). std::deque<> is better suited to represent a time series as using a contiguous container (e.g. vector)
///             would have an increasing cost of adding elements at large sizes (need to move the whole container elsewhere when capacity is reached). A deque has constant complexity (O1) when
///             inserting elements, at the cost of a slightly larger overhead at smaller sizes.
///             Since it's impossible to predict the length of any trial and Trigno sensors sample at high frequencies, a constant insert complexity is preferable to avoid a practical trial length limit.
///
class Sequence : public std::series< Frame, Frame::TimeStamp > /* a.k.a. std::sorted_indexer< Frame::Stamped > */ {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Specialization of std::range_iterator representing a range between sequential data frames.
    ///             Behaves like an *iterable* subcontainer-like iterator over input sequence.
    ///
    /// @note       Useful to encode information about a moving window when processing data on a Sequence object.
    ///
    /// @todo       Move to base class std::indexer<> or std::sorted_indexer<>.
    ///
    using Range = typename std::series< Frame, Frame::TimeStamp >::element_range< Frame::Stamped >;

    //--------------------------------------------------------------------------
    /// @brief      Specialization of std::range_iterator representing a range between sequential data frames (const overload).
    ///             Behaves like an *iterable* subcontainer-like iterator over input sequence.
    ///
    /// @note       Useful to encode information about a moving window when processing data on a Sequence object.
    ///
    /// @todo       Move to base class std::indexer<> or std::sorted_indexer<>.
    ///
    using ConstRange = typename std::series< Frame, Frame::TimeStamp >::const_element_range< Frame::Stamped >;

    //--------------------------------------------------------------------------
    /// @brief      Single-sensor value sequence type.
    ///
    /// @note       Useful when extracting sensor data for external tools.
    ///
    using Signal = std::series< Sample::Value, Frame::TimeStamp >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (empty) instance.
    ///
    Sequence() = default;

    //--------------------------------------------------------------------------
    /// @brief      Discards *n_samples* older samples (@ front of container).
    ///
    /// @param[in]  n_samples  Number of sample to discard. Defaults to 1.
    ///
    /// @todo       Declaring Sequence as a specialization of std::sorted_indexer<> removes the need for this method
    ///
    void discard(size_t n_samples = 1);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified frame, *copying* it into internal container.
    ///
    /// @param[in]  frame          Input data frame.
    /// @param[in]  sequential     Sequential check flag. If true, checks if input time stamp is incremental.
    ///                            Defaults to false (for versatility && performance reasons).
    /// @param[in]  match_sensors  Match content flag. If true, checks if input sensors match back of sequence.
    ///                            Defaults to false (for versatility && performance reasons).
    ///
    /// @throw      std::invalid_argument if *sequential* is true and *time* is not greater than last frame in sequence.
    /// @throw      std::invalid_argument if *match_sensors* is true and *frame* sensor list is not equal to this->back().sensors().
    ///
    /// @note       Runtime content check adds versatility, but also overhead, thus discouraged in performance-sensitive applications.
    ///             In such cases, an alternative is to load frames wo/ checking content and sorting afterwards w/ sort()
    ///
    void add(const Frame::Stamped& frame, bool sequential = false, bool match_sensors = false);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified frame, *moving* it into internal container.
    ///
    /// @param[in]  frame          Input (movable) data frame (as rvalue reference).
    /// @param[in]  sequential     Sequential check flag. If true, checks if input time stamp is incremental.
    ///                            Defaults to false (for versatility && performance reasons).
    /// @param[in]  match_sensors  Match content flag. If true, checks if input sensors match back of sequence.
    ///                            Defaults to false (for versatility && performance reasons).
    ///
    /// @throw      std::invalid_argument if *sequential* is true and *time* is not greater than last frame in sequence.
    /// @throw      std::invalid_argument if *match_sensors* is true and *frame* sensor list is not equal to this->back().sensors().
    ///
    /// @note       Runtime content check adds versatility, but also overhead, thus discouraged in performance-sensitive applications.
    ///             In such cases, an alternative is to load frames wo/ checking content and sorting afterwards w/ sort()
    ///
    void add(Frame::Stamped&& frame, bool sequential = false, bool match_sensors = false);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified frame with given time stamp, *copying* it into internal container.
    ///
    /// @param[in]  time           Time stamp.
    /// @param[in]  frame          Input data frame.
    /// @param[in]  sequential     Sequential check flag. If true, checks if input time stamp is incremental.
    ///                            Defaults to false (for versatility && performance reasons).
    /// @param[in]  match_sensors  Match content flag. If true, checks if input sensors match back of sequence.
    ///                            Defaults to false (for versatility && performance reasons).
    ///
    /// @throw      std::invalid_argument if *sequential* is true and *time* is not greater than last frame in sequence.
    /// @throw      std::invalid_argument if *match_sensors* is true and *frame* sensor list is not equal to this->back().sensors().
    ///
    /// @note       Runtime content check adds versatility, but also overhead, thus discouraged in performance-sensitive applications.
    ///             In such cases, an alternative is to load frames wo/ checking content and sorting afterwards w/ sort()
    ///
    void add(const Frame::TimeStamp& time, const Frame& frame, bool sequential = false, bool match_sensors = false);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified frame with given time stamp, *moving* it into internal container.
    ///
    /// @param[in]  time           Time stamp.
    /// @param[in]  frame          Input data frame.
    /// @param[in]  sequential     Sequential check flag. If true, checks if input time stamp is incremental.
    ///                            Defaults to false (for versatility && performance reasons).
    /// @param[in]  match_sensors  Match content flag. If true, checks if input sensors match back of sequence.
    ///                            Defaults to false (for versatility && performance reasons).
    ///
    /// @throw      std::invalid_argument if *sequential* is true and *time* is not greater than last frame in sequence.
    /// @throw      std::invalid_argument if *match_sensors* is true and *frame* sensor list is not equal to this->back().sensors().
    ///
    /// @note       Runtime content check adds versatility, but also overhead, thus discouraged in performance-sensitive applications.
    ///             In such cases, an alternative is to load frames wo/ checking content and sorting afterwards w/ sort()
    ///
    void add(const Frame::TimeStamp& time, Frame&& frame, bool sequential = false, bool match_sensors = false);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified frame, *copying* it into internal container.
    ///
    /// @param[in]  frame  Input data frame.
    ///
    /// @note       Equivalent to add(), provided for cleaner syntax and higher verbosity.
    ///
    Sequence& operator<<(const Frame::Stamped& frame);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified frame, *moving* it into internal container.
    ///
    /// @param[in]  frame  Input data frame (as move reference).
    ///
    /// @note       Equivalent to add(), provided for cleaner syntax and higher verbosity.
    ///
    /// @note       Overload provided to accept rvalue references as argument e.g. when fetching directly from a data client (frame << client.read()).
    ///
    Sequence& operator<<(Frame::Stamped&& frame);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified sequence range, *copying* it (i.e. all its frames) into internal container.
    ///
    /// @param[in]  range  Input data range (or sequence).
    ///
    /// @note       If input range has a large size, it becomes a costly operation.
    ///
    /// @note       Sequence is implicitly convertible to Sequence::ConstRange, thus operator overload is called when adding two Sequence instances.
    ///
    Sequence& operator<<(Sequence::ConstRange range);

    //--------------------------------------------------------------------------
    /// @brief      Adds the specified sequence, *moving* it (i.e. all its frames) into internal container.
    ///
    /// @param[in]  sequence  Input (movable) sequence (as rvalue reference).
    ///
    /// @note       By design, input *sequence* is moved/consumed into internal container, i.e. its contents are not available afterwards.
    ///
    Sequence& operator<<(Sequence&& sequence);

    //--------------------------------------------------------------------------
    /// @brief      Convenience function call operator overload, extracting transversal data for given *sensor_id* and *channel*.
    ///
    /// @param[in]  label    Sensor ID
    /// @param[in]  channel  Channel. Defaults to 0 (first channel).
    ///
    /// @return     ConstRange (read-only) with frames between *from* and *to*.
    ///
    Signal operator()(const sensor::ID& sensor_id, size_t channel = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Convenience function call operator overload, extracting transversal data for given *sensor_label* and *channel*.
    ///
    /// @param[in]  label    Sensor label
    /// @param[in]  channel  Channel. Defaults to 0 (first channel).
    ///
    /// @return     ConstRange (read-only) with frames between *from* and *to*.
    ///
    Signal operator()(const sensor::Label& sensor_label, size_t channel = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of base class member & overloads to avoid shadowing by Sequence's overload operator().
    ///
    using std::series< Frame, Frame::TimeStamp >::operator();

    //--------------------------------------------------------------------------
    /// @brief      Get an iterative & iteratable range of samples, from time interval (frame window).
    ///
    /// @param[in]  time        Time value to initialize iterator.
    /// @param[in]  window      Time interval .
    /// @param[in]  sample_rate Sample rate of input data. Required to estimate number of samples for each window.
    /// @param[in]  overlap     Overlap (number of samples) betwenn consecutive windows. Defaults to 0 (no overlap). Only meaningful when iterating range.
    ///
    /// @return     Instance of Range (a.k.a. std::range_iterator< Sequence >).
    ///
    /// @note       Can be used to iterate over data sequence with a moving/sliding window (cf. std::range_iterator)
    ///
    Range range(float from, float window, float sample_rate, float overlap = 0);

    //--------------------------------------------------------------------------
    /// @brief      Get a (const) iterative & iteratable range of samples, from time interval (frame window).
    ///
    /// @param[in]  time        Time value to initialize iterator.
    /// @param[in]  window      Size (number of samples) of the sample window.
    /// @param[in]  sample_rate Sample rate of input data. Required to estimate number of samples for each window.
    /// @param[in]  overlap     Overlap (number of samples) betwenn consecutive windows. Defaults to 0 (no overlap). Only meaningful when iterating resulting range.
    ///
    /// @return     Instance of Range (a.k.a. std::range_iterator< Sequence >).
    ///
    /// @note       Can be used to iterate over data sequence with a moving/sliding window (cf. std::range_iterator).
    ///
    ConstRange range(float from, float window, float sample_rate, float overlap = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of base class member & overloads to avoid shadowing by Sequence's overload range().
    ///
    using std::series< Frame, Frame::TimeStamp >::range;

    //--------------------------------------------------------------------------
    /// @brief      Conversion operator to Range (a.k.a. std::range_iterator).
    ///
    /// @return     Range instance encoding full sequence (thus, not iteratable).
    ///
    /// @note       Useful when pass full sequence to functions accepting a Range argument.
    ///
    operator Range();

    //--------------------------------------------------------------------------
    /// @brief      Conversion operator to ConstRange (a.k.a. std::range_iterator).
    ///
    /// @return     Range instance encoding full sequence (thus, not iteratable).
    ///
    /// @note       Useful when pass full sequence to functions accepting a Range argument.
    ///
    operator ConstRange() const;

    //--------------------------------------------------------------------------
    /// @brief      Builds a Signal instance holding channel data for given sensor *id* and given *channel*.
    ///             Useful to encode sensor/channel data to external non-trigno tools (e.g. plotting).
    ///
    /// @param[in]  range      Input range (data source).
    /// @param[in]  sensor_id  Sensor identifier.
    /// @param[in]  channel    Channel identifier. Defaults to 0 (first channel).
    ///
    /// @return     Instance of Signal with data for specified sensor/channel.
    ///
    static Signal extract(ConstRange range, sensor::ID id, size_t channel = 0);

    //--------------------------------------------------------------------------
    /// @brief      Builds a Signal instance holding channel data for given sensor *label* and given *channel*.
    ///             Useful to encode sensor/channel data to external non-trigno tools (e.g. plotting).
    ///
    /// @param[in]  range         Input range (data source).
    /// @param[in]  sensor_label  Sensor label.
    /// @param[in]  channel       Channel identifier. Defaults to 0 (first channel).
    ///
    /// @return     Instance of Signal with data for specified sensor/channel.
    ///
    static Signal extract(ConstRange range, const sensor::Label& sensor_label, size_t channel = 0);
};

}  // namespace trigno

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SEQUENCE_HPP_
