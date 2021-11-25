/// @file sensor.hpp
/// @brief This file is marvelous.

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SENSOR_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SENSOR_HPP_

#include <vector>
#include <string>

namespace trigno {

namespace sensor {

//------------------------------------------------------------------------------
/// @brief      Enumerator matching sensor IDs to positional 0-based indexes. Required as sensors are numbered from 1-upwards.
///
enum ID {
    _1 = 0,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _10,
    _11,
    _12,
    _13,
    _14,
    _15,
    _16,
    // ...
    MAX = _16
};


// class ID {

//     ID(size_t idx) { }


//     enum {
//         _1 = 0,
//         _2,
//         _3,
//         _4,
//         _5,
//         _6,
//         _7,
//         _8,
//         _9,
//         _10,
//         _11,
//         _12,
//         _13,
//         _14,
//         _15,
//         _16,
//         // ...
//         MAX = _16
//     };

//     operator int();
//     operator size_t();
// }



//------------------------------------------------------------------------------
/// @brief      Namespace-wide typedef sensor label type.
///
using Label = std::string;



//------------------------------------------------------------------------------
/// @brief      Namespace-wide typedef for a container of ID i.e. a descriptor of target/source sensors.
///
using List = std::vector< ID >;



//------------------------------------------------------------------------------
/// @brief      Namespace-wide typedef for a container of sensor labels i.e. a descriptor of target/source sensors.
///
using Labels = std::vector< Label >;



//------------------------------------------------------------------------------
/// @brief      Constructs a sensor list with the complete sensor set supported by Trigno Research+ station (i.e. 16 pairing positions)
///
/// @return     List instance with all sensor IDs.
///
inline static List _all() {
    List list;
    list.reserve(ID::MAX);
    for (int s = ID::_1; s <= ID::MAX; s++) {
        list.emplace_back(static_cast< ID >(s));
    }
    return list;
}



//------------------------------------------------------------------------------
/// @brief      Static instance of List with 
///
static const List all = _all();


//------------------------------------------------------------------------------
/// @brief      Searches given list of *sensors* for given *id*.
///
/// @param[in]  id       Sensor identifier (cf ID).
/// @param[in]  sensors  Sensor list to search in.
///
/// @return     True if *id* exists in *sensors* list, false otherwise.
///
inline bool find(ID id, const sensor::List& sensors) {
    for (const auto& sensor : sensors) {
        if (sensor == id) {
            return true;
        }
    }
    return false;
}

}  // namespace sensor

}  // namespace trigno



//------------------------------------------------------------------------------
/// @brief      Equality operator between trigno::sensor::List objects.
///
/// @param[in]  lhs   List @ left hand side of comparison
/// @param[in]  rhs   List @ right hand side of comparison
///
/// @return     The result of the equality. True if both lists contain same sensor ids in the same order, false otherwise.
///
/// @note       Declared outside trigno::sensor namespace for broader use scope.
///
inline bool operator==(const trigno::sensor::List& lhs, const trigno::sensor::List& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (int s = 0; s < lhs.size(); s++) {
        if (lhs[s] != rhs[s]) {
            return false;
        }
    }
    return true;
}

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SENSOR_HPP_
