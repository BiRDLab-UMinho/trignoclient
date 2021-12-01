//------------------------------------------------------------------------------
/// @file       basic_executor.hpp
/// @author     João André
///
/// @brief      Header file providing declaration & definition of std::basic_executor and std::basic_timed_excutor classes for custom multi-thread operation abstraction.
///
/// std::basic_executor and std::basic_timed_executor as abstract classes that provide a simple way to streamline development of threaded applications
/// while abstracting over <thread>/<mutex>/<atomic> implementations, for single and looped/timed use cases.
///
/// Derived types are required to override execute() base pure virtual, and both classes provide run() and launch()/wait() methods for synchronous and asynchronous workflow respectively.
///
/// std::basic_executor is intended for looped/sporadic operations, and std::basic_timed_executor extends its functionality by employing a timer that runs asynchronous to the main operation,
/// and executing operation repeatedly until timer expires. boost::asio library provides timer implementation.
///
/// Additionally, std::wait_for() and std::chain() are provided as free functions to manage execution lifespan of different executor types.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_BASICEXECUTOR_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_BASICEXECUTOR_HPP_

#include <chrono>
#include <atomic>
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <utility>
#include <boost/asio.hpp>

namespace std {

//------------------------------------------------------------------------------
/// @brief      Simple extensible base class for looped/unlooped multi-threaded operations.
///             Provides a consistent interface to develop synchronous and asynchronous operations with only superficial familiarity with multi-threaded programming.
///
/// @note       Execution is looped as:
///             \code
///             start();
///             while (active()) {
///                 execute();
///                 if (kill) return;
///             }
///             stop();
///             \endcode
///             Therefore, derived implementations *must* override execute() method (and optionally start() and stop()) as well as active() with execution condition.
///             Default base implementations are provided for start(), stop() and active() so that execute() is called once (single-time operations).
///
class basic_executor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    basic_executor();

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    virtual ~basic_executor();

    //--------------------------------------------------------------------------
    /// @brief      Run operation. Calls specialized/derived implementation of execute().
    ///
    virtual void run();

    //--------------------------------------------------------------------------
    /// @brief      Launches an operation asynchronously (in the background), not blocking/waiting for its completion.
    ///
    /// @note       Operation keeps executing independent of main thread, even if it exits/finishes @ different times.
    ///
    virtual void launch();

    //--------------------------------------------------------------------------
    /// @brief      Waits for execution to finish.
    ///
    /// @note       Returns immediately if thread was already terminated.
    ///
    virtual void wait() const;

    //--------------------------------------------------------------------------
    /// @brief      Forcibly stop execution @ next execution loop.
    ///
    /// @note       If execute() blocks thread, executor will not be killed.
    ///
    virtual void kill();

    //--------------------------------------------------------------------------
    /// @brief      Check if execution condition is verified.
    ///
    /// @return     True if active, false otherwise.
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///
    virtual bool active() const;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Un-looped function called before execute() loop.
    ///
    virtual void start();

    //--------------------------------------------------------------------------
    /// @brief      Un-looped function called after execute() loop.
    ///
    virtual void stop();

    //--------------------------------------------------------------------------
    /// @brief      Executes operation. Default implementation calls execute() in a loop while active() returns true i.e.
    ///             \code
    ///             start()
    ///             while (active()) {
    ///                 execute();
    ///                 if (kill) return;
    ///             }
    ///             stop()
    ///             \endcode
    ///
    /// @note       Pure virtual, derived types must provide their own implementation.
    ///
    virtual void execute() = 0;

    //--------------------------------------------------------------------------
    /// @brief      Future object holding execution status.
    ///
    future< void > _done;

    //--------------------------------------------------------------------------
    /// @brief      Kill flag, for shutting down executor from separate threads (e.g. when running asynchronously with launch()).
    ///
    atomic< bool > _kill;
};



inline basic_executor::basic_executor() : _kill(false) {
    /* ... */
}



inline basic_executor::~basic_executor() {
    /* ... */
}



inline void basic_executor::run() {
    start();
    while (active()) {
        execute();
        if (_kill) return;
    }
    stop();
}



inline void basic_executor::launch() {
    // call run() asynchronously on a separate thread
    _done = std::async(std::launch::async, [this] { run(); });
}



inline void basic_executor::wait() const {
    if (_done.valid()) {
        _done.wait();
    }
}



inline bool basic_executor::active() const {
    /* ... */
    // base implementation disables loop i.e. execute() is run only once
    return false;
}



inline void basic_executor::start() {
    /* ... */
}



inline void basic_executor::stop() {
    /* ... */
}


inline void basic_executor::kill() {
    /* ... */
    _kill = true;
    wait();
}


//------------------------------------------------------------------------------
/// @brief      Simple extensible base class for timed multi-threaded operations.
///             Extends std::basic_executor by implementing an operation deadline through the use of a timer.
///             Provides a consistent interface to develop synchronous and asynchronous operations with superficial familiarity with multi-threaded programming.
///
/// @note       The purpose is to provide a base type for different similar operations that allows easy management.
///
class basic_timed_executor : public basic_executor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Duration type. Can be used for higher verbosity when parsing time arguments (i.e. duration(1.5)).
    ///
    // using duration = std::chrono::duration< float, std::ratio< 1 > >;

    //--------------------------------------------------------------------------
    /// @brief      Timer type. Using boost::asio timers allows for greater felxiblity than std::chrono clocks (a timer class would need to be implemeted)
    ///
    using timer = boost::asio::steady_timer;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    basic_timed_executor();

    //--------------------------------------------------------------------------
    /// @brief      Run timed operation. Calls specialized/derived implementation of start(), execute() and stop().
    ///
    /// @param[in]  duration  Duration to run operation for (in seconds).
    ///
    void run(const chrono::milliseconds& time = chrono::milliseconds::max());

    //--------------------------------------------------------------------------
    /// @brief      Launches a timed operation asynchronously (in the background), not blocking/waiting for its completion.
    ///
    /// @param[in]  duration  Duration to run operation for (in seconds).
    ///
    /// @note       Operation keeps executing independent of main thread, even if it exits/finishes @ different times.
    ///
    void launch(const chrono::milliseconds& time = chrono::milliseconds::max());

    //--------------------------------------------------------------------------
    /// @brief      Waits for execution to finish.
    ///
    /// @note       Returns immediatly if thread was already terminated.
    ///
    void wait() const override;

    //--------------------------------------------------------------------------
    /// @brief      Check if executor is currently active (timer has elapsed).
    ///
    /// @return     True if active, false otherwise.
    ///
    bool active() const override;

    //--------------------------------------------------------------------------
    /// @brief      Access remaining time in execution.
    ///
    /// @return     Time remaining (in us).
    ///
    template < typename DurationType = chrono::milliseconds >
    DurationType remaining() const noexcept;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of run() in order to remove from public interface.
    ///
    using basic_executor::run;

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of launcher() in order to remove from public interface.
    ///
    using basic_executor::launch;

    //--------------------------------------------------------------------------
    /// @brief      Resets timer to *time* & starts counting.
    ///
    void reset(const chrono::milliseconds& time);

    //--------------------------------------------------------------------------
    /// @brief      Future instance holding result of timer call.
    ///
    future< void > _elapsed;

    //--------------------------------------------------------------------------
    /// @brief      IO handler that runs asynchronous timer.
    ///
    boost::asio::io_context _io;

    //--------------------------------------------------------------------------
    /// @brief      Timer object.
    ///
    timer _timer;
};



inline basic_timed_executor::basic_timed_executor() :
    _timer(_io) {
        /* ... */
}



inline void basic_timed_executor::run(const chrono::milliseconds& time) {
    // wait(); // wait for previous execution if active!
    reset(time);
    run();
}



inline void basic_timed_executor::launch(const chrono::milliseconds& time) {
    // wait(); // wait for previous execution if active!
    reset(time);
    launch();
}



template < typename DurationType >
inline DurationType basic_timed_executor::remaining() const noexcept {
    return std::chrono::duration_cast< DurationType >(_timer.expires_from_now());
}



inline bool basic_timed_executor::active() const {
    // test if std::future has valid state == timer async function has elapsed!
    return (_elapsed.wait_for(std::chrono::nanoseconds(0)) != std::future_status::ready);
}



inline void basic_timed_executor::reset(const chrono::milliseconds& time) {
    // reset timer and call async_wait
    _timer.expires_after(std::chrono::duration_cast< timer::duration >(time));
    _timer.async_wait([this](const boost::system::error_code&) { /* no need to do anything */ });
    // wait for timer to expire if currently running
    if (_elapsed.valid()) {
        _elapsed.wait();
    }
    // start io_run asynchronously
    _elapsed = std::async(std::launch::async, [this] { _io.run(); });
}



inline void basic_timed_executor::wait() const {
    // wait for timer to expire if currently running
    if (_elapsed.valid()) {
        _elapsed.wait();
    }
    // call base class method to wait for executer finish
    basic_executor::wait();
}



//------------------------------------------------------------------------------
/// @brief      Waits for the completion of given *executors*.
///
/// @param[in]  executors  Executor list.
///
/// @note       Provided for conveninece when handling multiple executors.
///
inline void wait_for(const std::initializer_list< basic_executor* >& executors) {
    for (const auto& executor : executors) {
        executor->wait();
    }
}



//------------------------------------------------------------------------------
/// @brief      Chains execution of given *executors*.
///
/// @param      executors   Executor list.
/// @param[in]  sequential  Whether executores are launched sequentially.
///                         If true, waits for the completion of each before launching the next.
///                         If false, all executors are started in parallel.
///
inline void chain(std::initializer_list< basic_executor* >& executors, bool sequential = false) {
    for (const auto& executor : executors) {
        executor->launch();
        if (sequential == true) {
            executor->wait();
        }
    }
}

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_BASICEXECUTOR_HPP_
