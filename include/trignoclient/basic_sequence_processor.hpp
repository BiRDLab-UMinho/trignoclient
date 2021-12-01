#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICSEQUENCEPROCESSOR_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_BASICSEQUENCEPROCESSOR_HPP_

#include <type_traits>
#include "std/basic_executor.hpp"  // std::basic_executor
#include "sequence.hpp"            // trigno::Sequence

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Basic type for sequence processing operations i.e. signal filtering or extracted features/metrics.
///
/// @note       Derives from std::basic_executor to simplify synchronous/asynchronous implementations.
///             Derived/specialized types only need to provide execute() implementation (range and sensor descriptor are declared as class members and are updated @ call)
///
class BasicSequenceProcessor : public std::basic_executor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    BasicSequenceProcessor();

    //--------------------------------------------------------------------------
    /// @brief      Run processing operation synchronously in given *range*.
    ///
    /// @param      range  Input range
    ///
    /// @note       Range passed by value in order to allow both lvalue and rvalue arguments without an additional overload.
    ///             Range (a.k.a.) represents a light iteraror type, copies are cheap.
    ///
    void run(Sequence::Range range, const sensor::List& sensors = sensor::all);

    //--------------------------------------------------------------------------
    /// @brief      Run processing operation asynchronously in given *range*.
    ///
    /// @param      range  Input range
    ///
    /// @note       Range passed by value in order to allow both lvalue and rvalue arguments without an additional overload.
    ///             Range (a.k.a.) represents a light iteraror type, copies are cheap.
    ///
    void launch(Sequence::Range range, const sensor::List& sensors = sensor::all);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of base type run() member as protected.
    ///
    using std::basic_executor::run;

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of base type launch() member as protected.
    ///
    using std::basic_executor::launch;

    //--------------------------------------------------------------------------
    /// @brief      Range to be processed
    ///
    Sequence::Range _range;

    //--------------------------------------------------------------------------
    /// @brief      Sensors to be processed
    ///
    sensor::List _sensors;
};



inline BasicSequenceProcessor::BasicSequenceProcessor() :
    std::basic_executor(),
    _range(nullptr) {
        /* ... */
}



inline void BasicSequenceProcessor::run(Sequence::Range range, const sensor::List& sensors) {
    _range = range;
    _sensors = sensors;
    std::basic_executor::run();

    // std::basic_executor::start();
    // while (std::basic_executor::active()) {
    //     execute(range, sensors);
    //     if (_kill) return;
    // }
    // std::basic_executor::stop();
}



inline void BasicSequenceProcessor::launch(Sequence::Range range, const sensor::List& sensors) {
    _range = range;
    _sensors = sensors;
    std::basic_executor::launch();
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
