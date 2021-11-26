//------------------------------------------------------------------------------
/// @file       aux_types.hpp
/// @author     João André
///
/// @brief      Header file providing declaration & definition of auxilary data types.
///             At this stage only trigno::Coordinates3D and trigno::Inertia are provided for convenience/reference.
///             AUXDataClient does not parse data in any way, it is up to users to handle received data appropriatly.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_AUXTYPES_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_AUXTYPES_HPP_

namespace trigno {

//--------------------------------------------------------------------------
/// @brief      Generic type describing 3D coordinates or axial values ('x', 'y' and 'z').
///             Useful when representing linear/spatial quatities.
///
struct Coordinates3D {
    //--------------------------------------------------------------------------
    /// @brief      Value along the 'X' axis .
    ///
    float x;

    //--------------------------------------------------------------------------
    /// @brief      Value along the 'Y' axis.
    ///
    float y;

    //--------------------------------------------------------------------------
    /// @brief      Value along the 'Z' axis.
    ///
    float z;

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  x     'X' value.
    /// @param[in]  y     'Y' value.
    /// @param[in]  z     'Z' value.
    ///
    explicit Coordinates3D(float X = 0.0, float Y = 0.0, float Z = 0.0);
};



//------------------------------------------------------------------------------
/// @brief      Generic type describing inertial data acquired with an IMU (or similar).
///             Useful when representing auxiliary data frames.
///
struct Inertia {
    //--------------------------------------------------------------------------
    /// @brief      Accelerometer readings.
    ///
    Coordinates3D acc;

    //--------------------------------------------------------------------------
    /// @brief      Gyroscope readings.
    ///
    Coordinates3D gyro;

    //--------------------------------------------------------------------------
    /// @brief      Magnetomoter readings.
    ///
    Coordinates3D mag;
};

/* .... */

}  // namespace trigno

#endif  // TRIGGNOCLIENT_INCLUDE_TRIGNOCLIENT_AUXTYPES_HPP_