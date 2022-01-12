#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICSEQUENCEPROCESSOR_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICSEQUENCEPROCESSOR_HPP_

#include <utility>
#include <type_traits>
#include "std/basic_executor.hpp"  // std::basic_executor
#include "core/sequence.hpp"       // trigno::Sequence

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Basic type for sequence processing operations i.e. signal filtering or extracted features/metrics.
///
/// @note       Derives from std::basic_executor to simplify synchronous/asynchronous implementations.
///             Derived/specialized types only need to provide execute() implementation (range and sensor descriptor are declared as class members and are updated @ call)
///
class BasicSequenceProcessor : public std::basic_executor< Sequence::Range&&, const sensor::List& > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Parent type (base implementation of threaded execution).
    ///
    /// @note       Provided for verbosity.
    ///
    using ProcessorBase = std::basic_executor< Sequence::Range&&, const sensor::List& >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    BasicSequenceProcessor();

    //--------------------------------------------------------------------------
    /// @brief      Run processing operation synchronously in given *range*.
    ///
    /// @param[in]  range    Input range
    /// @param[in]  sensors  Sensors on which to operate. Defaults to all sensors.
    ///
    /// @note       Range passed by value by iterator convention (cheap to copy).
    ///
    void run(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Run processing operation asynchronously in given *range*.
    ///
    /// @param[in]  range    Input range
    /// @param[in]  sensors  Sensors on which to operate. Defaults to all sensors.
    ///
    /// @note       Range passed by value by iterator convention (cheap to copy).
    ///
    void launch(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Declaration of base method start(), in order to allow templated derived types to specialize it.
    ///
    using ProcessorBase::start;

    //--------------------------------------------------------------------------
    /// @brief      Declaration of base method stop(), in order to allow templated derived types to specialize it.
    ///
    using ProcessorBase::stop;

    //--------------------------------------------------------------------------
    /// @brief      Declaration of base method execute(), in order to allow templated derived types to specialize it.
    ///
    using ProcessorBase::execute;

    //--------------------------------------------------------------------------
    /// @brief      Declaration of base method execute(), in order to allow templated derived types to specialize it.
    ///
    using ProcessorBase::active;
};



inline BasicSequenceProcessor::BasicSequenceProcessor() :
    ProcessorBase() {
        /* ... */
}



inline void BasicSequenceProcessor::run(Sequence::Range&& range, const sensor::List& sensors) {
    // forward input arguments to base implementation
    // cf. https://stackoverflow.com/a/32691106/2340002 as to why std::move() is used instead of std::forward<>()
    ProcessorBase::run(std::move(range), sensors);
}



inline void BasicSequenceProcessor::launch(Sequence::Range&& range, const sensor::List& sensors) {
    // forward input arguments to base implementation
    // cf. https://stackoverflow.com/a/32691106/2340002 as to why std::move() is used instead of std::forward<>()
    ProcessorBase::launch(std::move(range), sensors);
}



//------------------------------------------------------------------------------
/// @brief      Basic type for processing operations that output a result e.g. extracted features/metrics.
///
/// @tparam     T     Output value type. *Can't* be void.
///
template < typename T >
class BasicSequenceMetric : public BasicSequenceProcessor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Metric type.
    ///
    using Value = T;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    BasicSequenceMetric();

    //--------------------------------------------------------------------------
    /// @brief      Access computed value.
    ///
    /// @return     *const* reference to value member.
    ///
    /// @note       run() needs to have been called for the returned value to be usable.
    ///
    const T& value() const noexcept;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Metric computed value. To be assigned @ run() call.
    ///
    T _value;
};



template < typename T >
inline BasicSequenceMetric< T >::BasicSequenceMetric() :
    BasicSequenceProcessor() {
        /* ... */
}



template < typename T >
inline const T& BasicSequenceMetric< T >::value() const noexcept {
    return _value;
}

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICSEQUENCEPROCESSOR_HPP_
