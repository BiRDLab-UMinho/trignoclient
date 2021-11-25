//------------------------------------------------------------------------------
/// @file       not_implemented.hpp
/// @author     João André
///
/// @brief      Header file providing custom exception 'std::not_implemented'
///
/// std::not_implemented is a useful execption derived from std::logic_error intended to be used
/// while implementations are still being developed and not ready for deployment.
///
//------------------------------------------------------------------------------

#ifndef _TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_NOTIMPLEMENTED_HPP_
#define _TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_NOTIMPLEMENTED_HPP_

#include <string>
#include <stdexcept>

namespace std {

//------------------------------------------------------------------------------
/// @brief      This class describes a 'not implemented' exception to signal currently missing features.
///
class not_implemented : public logic_error {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Construct a new instance.
    ///
    explicit not_implemented(const std::string& what = "") : logic_error("[" + std::string(__func__) + "] Function not yet implemented" + (what.empty() ? (": " + what) : ".")) { /* ... */ }

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    // virtual ~not_implemented() throw() { /* ... */ }

    //--------------------------------------------------------------------------
    /// @brief      Get description string
    ///
    // virtual char const * what() const { return "Function not yet implemented."; }
};

}  // namespace std

#endif  // _TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_NOTIMPLEMENTED_HPP_
