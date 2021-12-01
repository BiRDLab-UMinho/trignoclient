//------------------------------------------------------------------------------
/// @file       io.hpp
/// @author     João André
///
/// @brief      Header file providing declaration of namespace-wide generic IO (& utility) functions & operator overloads.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_IO_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_IO_HPP_

#include <string>
#include <fstream>
#include <limits>
#include <vector>
#include "std/utils.hpp" // std::tokenize, std::print_into, std::from_string
#include "sample.hpp"    // trigno::Sample
#include "frame.hpp"     // trigno::Frame, trigno::Frame::Stamped
#include "sequence.hpp"  // trigno::Sequence

//------------------------------------------------------------------------------
// @note        default vaue delimiter, employed by left/right shift operators
//              conditional check allows user to compile trignoclient w/ a different delimiter
#ifndef DEFAULT_DELIMITER
#define DEFAULT_DELIMITER ','
#endif

//------------------------------------------------------------------------------
/// @brief      Checks if template argument type *T* is (or derives from) a trigno data structure.
///             Used throughout this file in I/O implementations.
///
/// @tparam     T     Object type to check.
///
/// @return     True if one of, or derived from, trigno::Sample, trigno::Frame, trigno::Frame::Stamped and trigo::Sequence.
///             False otherwise.
///
template < typename T >
constexpr bool is_trigno_data_type() {
    if constexpr (std::is_same< trigno::Sample, T >() ||
                  std::is_same< trigno::Frame, T >()  ||
                  std::is_same< trigno::Frame::Stamped, T >() ||
                  std::is_same< trigno::Sequence, T >() ||
                  std::is_same< trigno::Sequence::Range, T >()) {
        return true;
    }
    return false;
}


namespace trigno {

//------------------------------------------------------------------------------
/// @brief      Writes the (serialized) content of *data* to given output stream *ostream*.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  data       Data source i.e. Sample, Frame or Sequence instances.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data values.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure write() is only declared for trigno data structures.
///
/// @return     Modified output stream
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
std::ostream& write(std::ostream& ostream, const T& data, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Read (serialized) content from given input stream *istream* into *data*.
///
/// @param      ostream    Stream to read data from.
/// @param[in]  data       Target data type i.e. Sample, Frame or Sequence instances.
/// @param[in]  delimiter  Delimiter character, occurring in-between consecutive data values.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure read() is only declared for trigno data structures.
///
/// @return     Modified output stream
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
std::istream& read(std::istream& istream, T& data, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Constructs an header string labelling each data value in given *data* structure.
///
/// @param[in]  data       Data source i.e. Sample, Frame or Sequence instances.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
/// @param[in]  extra      Extra text with additional data to prefix/suffix generated labels.
///                        Default to empty string.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure header() is only declared for trigno data structures.
///
/// @return     Header/data descriptor string.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
std::string header(const T& data, char delimiter = DEFAULT_DELIMITER, const std::string& extra = "");

//------------------------------------------------------------------------------
/// @brief      Initializes an object of type *T* from given *header*.
///
/// @param[in]  header     Data header with labels (channels) to parse.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///                        
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure initialize() is only declared for trigno data structures.
///
/// @return     Instance of *T*.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
T initialize(const std::string& header, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Saves contents of *data* into a local file @ *path*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  data       Source data.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure write() is only declared for trigno data structures.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
void save(const std::string& path, const T& data, bool with_header = false, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Assigns values to *data* from a local file @ *path*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  data       Source data.
/// @param[in]  initialize Initialize flag. If true, *data* is reinitialized with data header @ first line. Defaults to true;
/// @param[in]  line       Line index. Data before *line* is consumed from file stream. Defaults to 1 i.e. first line.  
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure write() is only declared for trigno data structures.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
void load(const std::string& path, T& data, bool initialize = true, size_t line = 1, char delimiter = DEFAULT_DELIMITER);

}  // namespace trigno


//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for higher verbosity in output stream operations.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  data       Data source i.e. Sample, Frame or Sequence instances.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure '<<' operator is only declared for trigno data structures.
///
/// @return     Modified output stream.
///
/// @note       Wraps around write() wo/ additional delimiter character selection.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
std::ostream& operator<<(std::ostream& ostream, const T& data);

//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for higher verbosity in input stream operations.
///
/// @param      istream    Stream to read data from.
/// @param[in]  data       Target data type i.e. Sample, Frame or Sequence instances.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure '>>' operator is only declared for trigno data structures.
///
/// @return     Modified/consumed input stream.
///
/// @note       Wraps around write() wo/ additional delimiter character selection.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
std::istream& operator>>(std::istream& istream, T& data);

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_IO_HPP_
