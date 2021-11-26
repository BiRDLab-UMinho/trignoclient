//------------------------------------------------------------------------------
/// @file       duration.hpp
/// @author     João André
///
/// @brief      Header file providing namespace-wide typedef of <chrono> duration (milliseconds) type.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DURATION_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DURATION_HPP_

#include <chrono>

namespace trigno {

//--------------------------------------------------------------------------
/// @brief      Duration type. Can be used for higher verbosity when parsing time arguments (i.e. duration(1.5)).
///
/// @note       Declared outside any class declarations/definitions in order to force different types to use same time abstraction.
///             Convertible to time/duration type used by std::tcp_client and std::basic_executor.
/// 
/// @note       Forces different types to
///
// using duration = std::chrono::duration< float, std::ratio< 1 > >;
using Duration = std::chrono::milliseconds;

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DURATION_HPP_
