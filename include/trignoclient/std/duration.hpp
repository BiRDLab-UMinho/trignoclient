//------------------------------------------------------------------------------
/// @file       duration.hpp
/// @author     João André
///
/// @brief      Header file providing namespace-wide typedef of <chrono> duration (seconds) type
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DURATION_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DURATION_HPP_

#include <chrono>

namespace std {

//--------------------------------------------------------------------------
/// @brief      Duration type. Can be used for higher verbosity when parsing time arguments (i.e. duration(1.5)).
///
/// @note       Declared outside any class declarations/definitions in order to be available to different types that require time abstraction.
///
using duration = std::chrono::duration< float, std::ratio< 1 > >;

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_DURATION_HPP_
