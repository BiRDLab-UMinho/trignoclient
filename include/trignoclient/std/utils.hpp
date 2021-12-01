//------------------------------------------------------------------------------
/// @file       utils.hpp
/// @author     João André
///
/// @brief      Header file providing declaration and definition of free generic global functions within the std namespace
///             that provide higher-level functionality.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_UTILS_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_UTILS_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include "type_check.hpp"

namespace std {

//------------------------------------------------------------------------------
/// @brief      Split text string into multiple sub-strings according to a given delimiter character.
///
/// @param[in]  data       Input text string.
/// @param[in]  separator  Delimiter/separator character.
///
/// @return     Vector of words/tokens matching substrings delimited by separator char. Empty vector if separator is not found.
///
template < typename C = vector< string > >
inline C tokenize(const string& data, char separator = ",", bool single_split = false) {
    C words;
    size_t start;
    size_t end = 0;

    while ((start = data.find_first_not_of(separator, end)) != std::string::npos) {
        end = data.find(separator, start);
        words.push_back(data.substr(start, end - start));
        if (single_split && end != string::npos) {
            words.push_back(data.substr(end + 1));
            break;
        }
    }

    return words;
}



//------------------------------------------------------------------------------
/// @brief      Skips/consumes input stream until character
///
/// @param      istream  Input stream.
/// @param[in]  target   Stop character.
///
/// @return     Modified input stream.
///
/// @note       Wrapper function provided for convenience/verbosity/cleaner syntax.
///
inline istream& ignore_until(istream& istream, char target) {
    istream.ignore(std::numeric_limits< std::streamsize >::max(), target);
    return istream;
}



//------------------------------------------------------------------------------
/// @brief      Skips input stream until next line.
///
/// @param      istream  Input stream.
///
/// @return     Modified input stream.
///
/// @note       Wrapper function provided for convenience/verbosity/cleaner syntax.
///
inline istream& skipline(istream& istream) { 
    return ignore_until(istream, '\n');
}



//------------------------------------------------------------------------------
/// @brief      Templated static SFINAE check for iteratable types.
///             It employs std::can_apply<> (cf. type_check.hpp) while testing specifically for container/range features of a type *T*.
///
/// @note       At this stage, an iteratable range (a.k.a. "ranged container"):
///                 1) A Container type that meets STL's *Container* & *SequenceContainer* mandatory requirements holding objects of
///                 2) An 'indexable' type that provide a valid public non-const 'id' member.
///
template < class T >
using range_begin = decltype(declval<T>().begin());
template < class T >
using range_end = decltype(declval<T>().end());
template < class T >
using iterator_dereference = decltype(*(declval<T>().begin()));
template < class T >
using iterator_increment = decltype(++(declval<T>().begin()));
///
template < class T >
using has_begin = can_apply< range_begin, T >;
template < class T >
using has_end = can_apply< range_end, T >;
template < class T >
using iterator_can_be_dereferenced = can_apply< iterator_dereference, T >;
template < class T >
using iterator_can_be_incremented = can_apply< iterator_increment, T >;
///
template < typename T >
constexpr bool is_iteratable() { return (has_begin< T >() &&
                                         has_end< T >()   &&
                                         iterator_can_be_dereferenced< T >() &&
                                         iterator_can_be_incremented< T >()); }



//------------------------------------------------------------------------------
/// @brief      Generic formatted printer function, for range/container-like types.
///
/// @param[out] ostream    The stream to write data to
/// @param[in]  range      Input range/container. Left shift (<<) stream operator is called for each element.
/// @param[in]  delimiter  Delimiter text/character. Defaults to ','.
///
/// @tparam     T          Range/container type. Must be iteratable (provide begin & end dereferenceable iterators)
///
/// @return     Modified output stream.
///
/// @note       Offers performance advatanges over std::formatted<>, as it writes directly into the output stream,
///             and does not instantiate a new object.
///
template < typename T >
inline ostream& print_into(ostream& ostream, const T& input, char delimiter = ',') {
    if constexpr(!is_iteratable< T >()) {
        // for non-iteratable types
        ostream << input;
    } else {
        // for iteratable types (e.g. containers), recursive call
        auto begin = input.begin();
        auto end = input.end();

        if (begin != end) {
            print_into(ostream, *begin, delimiter);
            while (++begin != end) {
                ostream << delimiter;
                print_into(ostream, *begin, delimiter);
            }
        }
    }
    return ostream;
}



//------------------------------------------------------------------------------
/// @brief      Loads values from an input stream into a range/container.
///
/// @param[out] istream    The stream to read data from.
/// @param[in]  range      Output range. Right shift (>>) stream operator is called for each element.
/// @param[in]  delimiter  Delimiter text/character. Defaults to ','.
///
/// @tparam     Range      Range/container type. Must be iteratable (provide begin & end dereferenceable iterators)
///
/// @return     Modified (consumed) output stream.
///
template < typename T >
inline istream& load_from(istream& istream, T& input, char delimiter = ',') {
    if constexpr(!is_iteratable< T >()) {
        // for non-iteratable types
        istream >> input;
    } else {
        auto begin = input.begin();
        auto end = input.end();

        size_t valid_pos = 0;
        while (begin != end) {
            // istream >> *begin;
            load_from(istream, *begin, delimiter);
            if (!istream.good()) {
                istream.seekg(valid_pos, std::ios::beg); // reset position to last valid pos
                istream.clear();
            } else {
                begin++;
                valid_pos = istream.tellg();
            }
            // istream.ignore(std::numeric_limits< std::streamsize >::max(), delimiter);
            ignore_until(istream, delimiter);
            if (istream.eof()) {
                break;
            }
        }
    }
    return istream;
}



//------------------------------------------------------------------------------
/// @brief      Formatted pseudo-type holding delimiter char and reference/pointer to source range/container.
///
/// @tparam     T     Source data type i.e. container/range-like type.
///
/// @note       Resulting syntax is simple enough, but implementation of stream IO is defined outside of class,
///             directly into operator overloads, which may not be a particularly good practice.
///
/// @note       Common usage would be `std::cout << formatted(obj, ",") << std::endl`
///
template < typename T >
struct formatted {
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  data       Input data to print.
    /// @param[in]  delimiter  Delimiter character. defaults to ','
    ///
    explicit formatted(T* data, char delimiter = ',') :
        src(data), del(delimiter) { /* ... */ }

    operator std::string() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();  // should be std::move(ss).str()?
    }

    template < typename U >
    friend std::ostream& operator<<(std::ostream& ostream, const std::formatted< U >& f);

    template < typename U >
    friend std::istream& operator>>(std::istream& istream, std::formatted< U >&& f);

 private:
    T* src;
    char del;
};




//------------------------------------------------------------------------------
/// @brief      Left shift / output stream operator overload for std::formatted.
///
/// @param      ostream  The stream to write data to.
/// @param[in]  f        Instance of std::formatted to print.
///
/// @tparam     U        Underlying range/container type.
///
/// @return     Modified output stream.
///
template < typename U >
inline std::ostream& operator<<(std::ostream& ostream, const std::formatted< U >& f) {
    return std::print_into(ostream, *(f.src), f.del);
}



//------------------------------------------------------------------------------
/// @brief      Right shift / input stream operator overload for std::formatted.
///
/// @param      istream  The stream to load data from.
/// @param[in]  f        Instance of std::formatted to print.
///
/// @tparam     U        Underlying range/container type.
///
/// @return     Modified input stream.
///
template < typename U >
inline std::istream& operator>>(std::istream& istream, std::formatted< U >&& f) {
    return std::load_from(istream, *(f.src), f.del);
}

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_UTILS_HPP_
