//------------------------------------------------------------------------------
/// @file       iterative.hpp
/// @author     João André
///
/// @brief      Header file providing declaration & definition of tringo::tools::Iterative<> executor type.
///
/// trigno::tools::Iterative<> wraps execution of sequence processors (i.e. derived from BasicSequenceProcessor class) iterative over a static or dynamic sequence.
/// It prevents sequence processors from having to deal with iteration and concurrent modification of input sequence (e.g. online processing), thus simplifying their implementation.
///
/// it is customizable over processing range (single, window or bulk processing) and performs an idle wait for new data whenever range can't be iterated.
/// Relies on trigno::Sequence::Range (a.k.a. std::range_iterator< Sequence >) functionality.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_ITERATIVE_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_ITERATIVE_HPP_

#include <thread>
#include <utility>
#include "std/type_check.hpp"                  // std::can_apply
#include "core/duration.hpp"                   // trigno::Duration
#include "core/sequence.hpp"                   // trigno::Sequence
// #include "core/io.hpp"
#include "tools/basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor, trigno::tools::BasicSequenceMetric<>

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Adapter class template that iterates any type derived BasicSequenceProcessor over input sequence/range, which includes cases where input is still being populated.
///             Waits *idle_time* for new frames when it runs out of data (i.e. range can't be iterated).
///
/// @tparam     Processor  Sequence processor type (derived from BasicSequenceProcessor) to iterate over input range.
/// @tparam     Step       Step (number of in-/decrements) of each iteration. Defaults to single increment, useful if larger steps or backward iteration is required.
///
/// @note       Provides basic live iterative behaviour for sequence processors. Abstracts processing operation from dealing with sequence iteration and/or online operations.
///
/// @note       Designed to be used with processing operations that do not compute any return. In order to compile sequential processing returns, use Filter class template.
///
template < typename Processor >
class Iterative : public BasicSequenceProcessor {
    //--------------------------------------------------------------------------
    static_assert(std::is_base_of< BasicSequenceProcessor, Processor >(),
                  "[Processor] MUST BE DERIVED FROM [BasicSequenceProcessor].");
    // static_assert(std::returns_something< T >() && std::is_constructible< Out::value_type, std::out_type< T > >(),
    //               "[Out::value_type] MUST BE CONVERTIBLE FROM [T::Value].");

 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  args       Arguments passed to T constructor.
    ///
    /// @tparam     Args       Variadic template argument describing argument parameter pack.
    /// @tparam     <unnamed>  SFINAE check over parameter pack - constructor is disabled if T(Args..) is not declared/defined.
    ///
    template < typename... Args, typename = typename std::enable_if< std::is_constructible< Processor, Args... >::value >::type >
    explicit Iterative(Args&&... args);

    //--------------------------------------------------------------------------
    /// @brief      Get idle duration.
    ///
    /// @return     Const reference to member trigno::Duration object.
    ///
    const Duration& idle() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Set idle duration.
    ///
    /// @param[in]  idle_time  Time duration (as trigno::Duration) to block execution @ end of input data.
    ///
    void idle(const Duration& idle_time);

    //--------------------------------------------------------------------------
    /// @brief      Get iteration step.
    ///
    int step() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Set iteration step.
    ///
    /// @param[in]  step_  Iteration step to set. Default value is 1 (set @ instantiation), which is the most common use case.
    ///
    void step(int step_);

    //--------------------------------------------------------------------------
    /// @brief      Access underlying processor type. Useful for runtime configuration.
    ///
    /// @return     Reference to member Processor instance.
    ///
    Processor& get() noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Run processing operation synchronously *from* given *range*.
    ///
    /// @param[in]  range    Input starting range. Copied to member range, which is in-/decremented until no more data is available (cf. active()).
    /// @param[in]  sensors  Sensors on which to operate. Defaults to all sensors.
    ///
    /// @note       Range passed by value by iterator convention (cheap to copy).
    ///
    /// @note       Alternatively, active() could be overriden to accept a range and test if it could be implemented, but current approach
    ///             is simpler, and has little to no performance overhead (iterators are cheap to copy).
    ///
    void run(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Un-looped function called before execute() loop.
    ///
    void start() override;

    //--------------------------------------------------------------------------
    /// @brief      Un-looped function called after execute() loop.
    ///
    void stop() override;

    //--------------------------------------------------------------------------
    /// @brief      Check if execution condition - range can be in-/decremented - is valid, i.e. wether there frames left in source sequence.
    ///
    /// @return     True if active, false otherwise.
    ///
    bool active() const override;

    //--------------------------------------------------------------------------
    /// @brief      Executes operation, delegating to Processor execute() implementation.
    ///
    /// @param[in]  range    Placeholder/dummy range. Ignored, member _range is used.
    /// @param[in]  sensors  Sensor list passed to Processor::execute()
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Checks if given range can be incremented, or is the last range in source sequence.
    ///
    /// @param[in]  range  Range to test.
    /// @param[in]  step   Increment step. Defaults to +1.
    ///
    /// @return     True if range can be incremented, false otherwise.
    ///
    bool incrementable(const Sequence::Range& range, int step = 1) const;

    //--------------------------------------------------------------------------
    /// @brief      Range iterator.
    ///
    Sequence::Range _range;

    //--------------------------------------------------------------------------
    /// @brief      Executor instance.
    ///
    Processor _executor;

    //--------------------------------------------------------------------------
    /// @brief      Iteration step.
    ///
    int _step;

    //--------------------------------------------------------------------------
    /// @brief      Idle time @ end of source data.
    ///
    /// @note       Wait is performed once, if no new data after it exits.
    ///
    Duration _idle_time;
};



template < typename Processor >
template < typename... Args, typename >
Iterative< Processor >::Iterative(Args&&... args) :
    BasicSequenceProcessor(),
    _range(nullptr),
    _executor(std::forward< Args >(args)...),
    _step(1),
    _idle_time(Duration(2000)) /* 2s default idle duration */ {
        /* ... */
        // disable base kill switch (BasicSequenceProcessor assumes single-time operation)
        _kill = false;
}



template < typename Processor >
const Duration& Iterative< Processor >::idle() const noexcept {
    return _idle_time;
}



template < typename Processor >
void Iterative< Processor >::idle(const Duration& idle_time) {
    _idle_time = idle_time;
}


template < typename Processor >
int Iterative< Processor >::step() const noexcept {
    return _step;
}



template < typename Processor >
void Iterative< Processor >::step(int step_) {
    _step = step_;
}



template < typename Processor >
Processor& Iterative< Processor >::get() noexcept {
    return _executor;
}



template < typename Processor >
void Iterative< Processor >::run(Sequence::Range&& range, const sensor::List& sensors) {
    // assign range to member iterator
    _range = range;
    // base execution loop
    start();
    do {
        // run executor operation
        // run() is called instead of launch() to force executor to behave synchronously w/ execution thread,
        // operations will still be run asynchronously if Iterative< Processor >::launch() is used.
        _executor.run(std::move(_range), sensors);
        // increment member range iterator
        // @todo pass arguments to active(), then there is no need to have a member _range
        //       although it is not advisable to forward/move more than once -> it can be change/destroyed within execute() 
        _range += _step;
    } while (active());
    stop();
}



template < typename Processor >
bool Iterative< Processor >::active() const {
    if (!incrementable(_range, _step)) {
        std::this_thread::sleep_for(_idle_time);
        return incrementable(_range, _step);
    }
    return true;
}



template < typename Processor >
bool Iterative< Processor >::incrementable(const Sequence::Range& range, int step_) const {
    auto next = (step_ >= 0) ? (range + step_) : (range - step_);
    if (range == next || next.size() == 0) {
        return false;
    }
    return true;
}



template < typename Processor >
void Iterative< Processor >::execute(Sequence::Range&& range, const sensor::List& sensors) {
    // default empty definition, can be used to extend execution
}



template < typename Processor >
void Iterative< Processor >::start() {
    BasicSequenceProcessor::start();
    // ...
}



template < typename Processor >
void Iterative< Processor >::stop() {
    BasicSequenceProcessor::stop();
    // ...
}

}  // namespace trigno::tools


namespace std {
//------------------------------------------------------------------------------
/// @brief      Templated static SFINAE check to identify specific metric type traits, to ensure it can be used with Filter class template.
///
template <class T>
using value_type = typename T::Value;
///
template < class T >
using has_value_type = can_apply< value_type, T >;
///
template < typename T >
constexpr bool has_public_type() { return (has_value_type< T >()); }

}  // namespace std


namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Simple filter implementation that iteratively computes a sequence metric and builds an output sequence.
///
/// @tparam     Metric     Sequence metric type that provides filtered values. *Must* be derived from BasicSequenceMetric.
///
/// @note       Sequence range (i.e. filter window) is set @ each call of run() (cf. Iterative). Filter will start at given range and iterate until end of input sequence.
///             This allows versatility and modularity over multiple range sizes, and abstract filter implementation over dealing with range iteration.
///
template < typename Metric >
class Filter : public Iterative< Metric > {
    //--------------------------------------------------------------------------
    /// @brief      Static SFINAE implementation that checks if Metric has a public value type.
    ///
    static_assert(std::has_public_type< Metric >(),
                  "INVALID METRIC TYPE!");

    //--------------------------------------------------------------------------
    /// @brief      Static SFINAE implementation that checks if Metric is derived from BasicSequenceMetric template.
    ///
    static_assert(std::is_base_of< BasicSequenceMetric< typename Metric::Value >, Metric >(),
                  "[T] MUST BE DERIVED FROM [BasicSequenceProcessor]!");

 public:
    //--------------------------------------------------------------------------
    /// @brief      Output sequence/container type.
    ///
    /// @note       If P::Value is trigno::Frame, Output is a trigno::Sequence (a.k.a. std::series< Frame, Frame::TimeStamp >).
    ///
    using Output = std::series< typename Metric::Value, Frame::TimeStamp >;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  args       Arguments passed to T constructor.
    ///
    /// @tparam     Args       Variadic template argument describing argument parameter pack.
    /// @tparam     <unnamed>  SFINAE check over parameter pack - constructor is disabled if T(Args..) is not declared/defined.
    ///
    template < typename... Args, typename = typename std::enable_if< std::is_constructible< Metric, Args... >::value >::type >
    explicit Filter(Args&&... args);

    //--------------------------------------------------------------------------
    /// @brief      Get output (filtered) sequence.
    ///
    /// @return     *const* reference to member sequence.
    ///
    /// @note       Output sequence is empty until run() is called.
    ///
    const Output& out() const noexcept;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Executes operation, i.e. calls run() on member executor instance and appends its result to the output container (if output type *not* void).
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Out data sequence.
    ///
    /// @note       If underlying sequence metric is of trigno::Frame, Filter output will be a trigno::Sequence.
    ///
    Output _out;

    //--------------------------------------------------------------------------
    /// @brief      Executor type, inherited from Iterative<> base class.
    ///
    using Iterative< Metric >::_executor;
};



template < typename Metric >
template < typename... Args, typename >
Filter< Metric >::Filter(Args&&... args) :
    Iterative< Metric >() {
        /* ... */
}



template < typename Metric >
const typename Filter< Metric >::Output& Filter< Metric >::out() const noexcept {
    return _out;
}



template < typename Metric >
void Filter< Metric >::execute(Sequence::Range&& range, const sensor::List& sensors) {
    // @note std::move is prefered here over std::forward<> as it is cleaner (no need to specify type),
    // and it is already known that an rvalue is needed.
    Iterative< Metric >::execute(std::move(range), sensors);
    // Output must be a STL or STL-like sequence container!
    _out.emplace_back(_executor.out());
}

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_ITERATIVE_HPP_
