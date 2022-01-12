//------------------------------------------------------------------------------
/// @file       sequence_metrics.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of sequence processor classes Maximum, Minimum, Average and RMS, specializing BasicSequenceProcessor.
///
/// Implemented processor types declare a trigno::frame output type, with computed/processed metrics for each sensor in input range. Additionally, declare a public typedef
/// with Iterative<> specializations to processor types, which implements a filter over input data (iteratively calls processor over customizable range).
//
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SEQUENCEMETRICS_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SEQUENCEMETRICS_HPP_

#include "tools/basic_sequence_processor.hpp"   // trigno::tools::BasicSequenceProcessor
#include "tools/iterative.hpp"                  // trigno::tools::Iterative

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Sequence processor that extracts
///
class Maximum : public BasicSequenceMetric< trigno::Frame > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Continuous/iterative processor type.
    ///
    using Filter = Iterative< Maximum >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    Maximum() = default;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Computes maximum value for sensor N channel #
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors) override;
};



//------------------------------------------------------------------------------
/// @brief      Sequence processor that extracts
///
class Minimum : public BasicSequenceMetric< trigno::Frame > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Continuous/iterative processor type.
    ///
    using Filter = Iterative< Minimum >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    Minimum() = default;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Computes maximum value for sensor N channel #
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors) override;
};



//------------------------------------------------------------------------------
/// @brief      Sequence processor that extracts
///
class Average : public BasicSequenceMetric< trigno::Frame > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Continuous/iterative processor type.
    ///
    using Filter = Iterative< Average >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    Average() = default;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Computes average value for sensor N channel #
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors) override;
};



//------------------------------------------------------------------------------
/// @brief      Simple RMS window filter.
///
/// @note       Useful when pre-processing EMG data i.e. extracting the envelope curve.
///
class RMS : public BasicSequenceMetric< trigno::Frame > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Continuous/iterative processor type.
    ///
    using Filter = Iterative< RMS >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    RMS() = default;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Executes filer/processing operation.
    ///
    /// @note       Overrides base implementation.
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors) override;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_RMS_HPP_
