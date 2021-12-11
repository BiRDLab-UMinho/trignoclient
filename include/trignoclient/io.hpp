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
#include <type_traits>
#include "std/utils.hpp" // std::tokenize, std::print_into, std::load_from
#include "sample.hpp"    // trigno::Sample
#include "frame.hpp"     // trigno::Frame, trigno::Frame::Stamped
#include "sequence.hpp"  // trigno::Sequence

//------------------------------------------------------------------------------
// @note        Default vaue delimiter, employed by left/right shift operators
//              Conditional check allows user to compile trignoclient w/ a different default delimiter
//
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
                  std::is_same< trigno::Sequence, T >()) {
        return true;
    }
    return false;
}


namespace trigno {

//------------------------------------------------------------------------------
/// @brief      Writes the (serialized) content of given *sample* to given output stream *ostream*.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  sample     Data source.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data values.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @return     Modified output stream
///
std::ostream& write(std::ostream& ostream, const Sample& sample, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Writes the (serialized) content of given *frame* to given output stream *ostream*.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  frame      Data source.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data values.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @return     Modified output stream
///
std::ostream& write(std::ostream& ostream, const Frame& frame, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Writes the (serialized) content of given *stamped frame* to given output stream *ostream*.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  frame      Data source.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data values.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @return     Modified output stream
///
std::ostream& write(std::ostream& ostream, const Frame::Stamped& frame, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Writes the (serialized) content of given sequence *range* to given output stream *ostream*.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  range      Data source.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data values.
///                        Defaults to DEFAULT_DELIMITER.
/// @param[in]  break_line Break line flag. If true (default), a line break is added between frames.
///
/// @return     Modified output stream
///
/// @note       Sequence is implicity convertible to ConstRange, thus instances will bind to *range* argument.
///
/// @note       By design, iterator types (e.g. Sequence::Range and Sequence::ConstRange) are cheap to copy and thus passed-by-value.
///
std::ostream& write(std::ostream& ostream, Sequence::ConstRange range, char delimiter = DEFAULT_DELIMITER, bool break_line = true);

//------------------------------------------------------------------------------
/// @brief      Read (serialized) content from given input stream *istream* into *sample*.
///
/// @param      ostream      Stream to read data from.
/// @param[in]  sample       Target Sample instance.
/// @param[in]  delimiter    Delimiter character, occurring in-between consecutive data values.
///                          Defaults to DEFAULT_DELIMITER.
/// @param[in]  ignore_break If true, interprets a line break ('\n') as a delimiter, loading from multiple lines in input stream.
///                          Otherwise (default), will stop at end of line.
///
/// @return     Modified/consumed input stream.
///
std::istream& read(std::istream& istream, Sample& sample, char delimiter = DEFAULT_DELIMITER, bool ignore_break = false);

//------------------------------------------------------------------------------
/// @brief      Read (serialized) content from given input stream *istream* into *frame*.
///
/// @param      ostream      Stream to read data from.
/// @param[in]  frame        Target Frame instance.
/// @param[in]  delimiter    Delimiter character, occurring in-between consecutive data values.
///                          Defaults to DEFAULT_DELIMITER.
/// @param[in]  ignore_break If true, interprets a line break ('\n') as a delimiter, loading from multiple lines in input stream.
///                          Otherwise (default), will stop at end of line.
///
/// @return     Modified/consumed input stream.
///
std::istream& read(std::istream& istream, Frame& frame, char delimiter = DEFAULT_DELIMITER, bool ignore_break = false);

//------------------------------------------------------------------------------
/// @brief      Read (serialized) content from given input stream *istream* into *stamped frame*.
///
/// @param      ostream      Stream to read data from.
/// @param[in]  frame        Target time-stamped frame a.k.a. Frame::Stamped instance.
/// @param[in]  delimiter    Delimiter character, occurring in-between consecutive data values.
///                          Defaults to DEFAULT_DELIMITER.
/// @param[in]  ignore_break If true, interprets a line break ('\n') as a delimiter, loading from multiple lines in input stream.
///                          Otherwise (default), will stop at end of line.
///
/// @return     Modified/consumed input stream.
///
std::istream& read(std::istream& istream, Frame::Stamped& frame, char delimiter = DEFAULT_DELIMITER, bool ignore_break = false);

//------------------------------------------------------------------------------
/// @brief      Read (serialized) content from given input stream *istream* into frame *range*.
///
/// @param      ostream      Stream to read data from.
/// @param[in]  frame        Target Sequence::Range instance.
/// @param[in]  delimiter    Delimiter character, occurring in-between consecutive data values.
///                          Defaults to DEFAULT_DELIMITER.
/// @param[in]  ignore_break If true, interprets a line break ('\n') as a delimiter, loading from multiple lines in input stream.
///                          Otherwise (default), will stop at end of line.
///
/// @return     Modified/consumed input stream.
///
/// @note       By design, iterator types (e.g. Sequence::Range and Sequence::ConstRange) are cheap to copy and thus passed-by-value.
///
std::istream& read(std::istream& istream, Sequence::Range range, char delimiter = DEFAULT_DELIMITER, bool ignore_break = false);

//------------------------------------------------------------------------------
/// @brief      Constructs a descriptor string labelling each data value in given *sample*.
///
/// @param[in]  sample     Source Sample instance.
/// @param[in]  name       Extra text with additional data to prefix/suffix generated labels.
///                        Default to empty string.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @return     Header/data descriptor string.
///
std::string descriptor(const Sample& sample, const std::string& name = "", char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Constructs a descriptor string labelling each data value in given *frame*.
///
/// @param[in]  frame      Source Frame instance.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
/// @return     Header/data descriptor string.
///
std::string descriptor(const Frame& frame, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Constructs a descriptor string labelling each data value in given stamped *frame*.
///
/// @param[in]  frame      Source stamped frame a.k.a. Frame::Stamped instance.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @return     Header/data descriptor string.
///
std::string descriptor(const Frame::Stamped& frame, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Initializes an object of type *T* from given *header*.
///
/// @param[in]  header     Data header with labels (channels) to parse.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure initialize() is only declared for trigno data structures.
///
/// @return     Newly constructed/initialized (empty) instance of *T*.
///
/// @note       trigno::initialize is *only* specialized for Sample, Frame & Frame::Stamped.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
T initialize(const std::string& header, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Initializes an object of type *T* from given input stream *istream*.
///
/// @param[in]  istream    Input stream to extract header data.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure initialize() is only declared for trigno data structures.
///
/// @return     Newly constructed/initialized (empty) instance of *T*.
///
/// @note       trigno::initialize is *only* specialized for Sample, Frame & Frame::Stamped.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
T initialize(std::istream& istream, size_t line, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Saves contents of *sample* into a local file @ *path*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  data       Source Sample instance.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
void save(const std::string& path, const Sample& sample, bool with_header = false, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Saves contents of *frame* into a local file @ *path*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  data       Source Frame instance.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
void save(const std::string& path, const Frame& frame, bool with_header = false, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Saves contents of stamped *frame* into a local file @ *path*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  data       Source Frame::Stamped instance.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
void save(const std::string& path, const Frame::Stamped& frame, bool with_header = false, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Saves contents of sequence *range* into a local file @ *path*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  data       Source Sequence::ConstRange (or Sequence) instance.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
/// @param[in]  break_line Break line flag. If true (default), a line break is added between frames.
///
/// @note       By design, iterator types (e.g. Sequence::Range and Sequence::ConstRange) are cheap to copy and thus passed-by-value.
///
void save(const std::string& path, Sequence::ConstRange range, bool with_header = false, char delimiter = DEFAULT_DELIMITER, bool break_line = true);

//------------------------------------------------------------------------------
/// @brief      Assigns values from a local file @ *path* to a existing *sample*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  sample     Target Sample instance.
/// @param[in]  line       Line index. Data before *line* is consumed from file stream. Defaults to 1 i.e. first line after header.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
void load(const std::string& path, Sample& sample, size_t line = 1, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Assigns values from a local file @ *path* to a existing *frame*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  frame      Target Frame instance.
/// @param[in]  line       Line index. Data before *line* is consumed from file stream. Defaults to 1 i.e. first line after header.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
void load(const std::string& path, Frame& frame, size_t line = 1, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Assigns values from a local file @ *path* to a existing *frame*.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  frame      Target Frame instance.
/// @param[in]  line       Line index. Data before *line* is consumed from file stream. Defaults to 1 i.e. first line after header.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
void load(const std::string& path, Frame::Stamped& frame, size_t line = 1, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Assigns values from a local file @ *path* to a *range* of frames on an existing Sequence.
///
/// @param[in]  path       Path to target local file.
/// @param[in]  range      Target Sequence::Range (or Sequence).
/// @param[in]  line       Line index. Data before *line* is consumed from file stream. Defaults to 1 i.e. first line after header.
/// @param[in]  delimiter  Delimiter character, placed in-between consecutive data labels.
///                        Defaults to DEFAULT_DELIMITER.
///
/// @note       By design, iterator types are cheap to copy and thus passed-by-value.
///
void load(const std::string& path, Sequence::Range range, size_t line = 1, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Creates a new *T* instance with data from a given input stream *istream*.
///
/// @param[in]  istream      Input stream.
/// @param[in]  data_line    Line index. All data before *line* is skipped/ignored.
///                          Defaults to 1 i.e. second line (first after header).
/// @param[in]  header       Header string to use to initialize data. Must match file contents.
///                          Defaults to empty string i.e. first file line is used to initialize data structures.
/// @param[in]  header_line  Index of line with data header, in case of empty *header*, used to initialize output.
///                          Defaults to 0 i.e. first line.
/// @param[in]  delimiter    Delimiter character, placed in-between consecutive data labels.
///                          Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure write() is only declared for trigno data structures.
///
/// @return     Newly constructed/initialized instance of *T*.
///
/// @note       trigno::initialize is *only* specialized for Sample, Frame, Frame::Stamped and Sequence.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
T load(std::istream& istream, size_t data_line = 1, const std::string& header = "", size_t header_line = 0, char delimiter = DEFAULT_DELIMITER);

//------------------------------------------------------------------------------
/// @brief      Creates a new *T* instance with data from a local file @ *path*.
///
/// @param[in]  path         Path to target local file.
/// @param[in]  data_line    Line index. All data before *line* is skipped/ignored.
///                          Defaults to 1 i.e. second line (first after header).
/// @param[in]  header       Header string to use to initialize data. Must match file contents.
///                          Defaults to empty string i.e. first file line is used to initialize data structures.
/// @param[in]  header_line  Index of line with data header, in case of empty *header*, used to initialize output.
///                          Defaults to 0 i.e. first line.
/// @param[in]  delimiter    Delimiter character, placed in-between consecutive data labels.
///                          Defaults to DEFAULT_DELIMITER.
///
/// @tparam     T          Data type i.e. Sample, Frame, Frame::Stamped or Sequence.
/// @tparam     <unnamed>  SFINAE check over *T* to ensure write() is only declared for trigno data structures.
///
/// @return     Newly constructed/initialized instance of *T*.
///
/// @note       trigno::initialize is *only* specialized for Sample, Frame, Frame::Stamped and Sequence.
///
template < typename T, typename = typename std::enable_if< is_trigno_data_type< T >() >::type >
T load(const std::string& path, size_t data_line = 1, const std::string& header = "", size_t header_line = 0, char delimiter = DEFAULT_DELIMITER);

}  // namespace trigno


//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for higher verbosity in output stream operations.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  sample     Source Sample instance.
///
/// @return     Modified output stream.
///
/// @note       Wraps around write() wo/ additional delimiter character selection.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::Sample& sample);

//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for higher verbosity in output stream operations.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  frame      Source Frame instance.
///
/// @return     Modified output stream.
///
/// @note       Wraps around write() wo/ additional delimiter character selection.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::Frame& frame);

//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for higher verbosity in output stream operations.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  frame      Source Frame::Stamped instance.
///
/// @return     Modified output stream.
///
/// @note       Wraps around write() wo/ additional delimiter character selection.
///
std::ostream& operator<<(std::ostream& ostream, const trigno::Frame::Stamped& frame);

//------------------------------------------------------------------------------
/// @brief      Left shift operator overload, for higher verbosity in output stream operations.
///
/// @param      ostream    Stream to write data to.
/// @param[in]  range      Source Sequence::ConstRange (or Sequence) instance.
///
/// @return     Modified output stream.
///
/// @note       Wraps around write() wo/ additional delimiter character selection.
///
std::ostream& operator<<(std::ostream& ostream, trigno::Sequence::ConstRange range);

//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for higher verbosity in input stream operations.
///
/// @param      istream    Stream to read data from.
/// @param[in]  sample     Target Sample instance.
///
/// @return     Modified/consumed input stream.
///
/// @note       Wraps around read() wo/ additional delimiter character selection.
///
std::istream& operator>>(std::istream& istream, trigno::Sample& sample);

//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for higher verbosity in input stream operations.
///
/// @param      istream    Stream to read data from.
/// @param[in]  frame      Target Frame instance.
///
/// @return     Modified/consumed input stream.
///
/// @note       Wraps around read() wo/ additional delimiter character selection.
///
std::istream& operator>>(std::istream& istream, trigno::Frame& frame);

//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for higher verbosity in input stream operations.
///
/// @param      istream    Stream to read data from.
/// @param[in]  frame      Target Frame::Stamped instance.
///
/// @return     Modified/consumed input stream.
///
/// @note       Wraps around read() wo/ additional delimiter character selection.
///
std::istream& operator>>(std::istream& istream, trigno::Frame::Stamped& frame);

//------------------------------------------------------------------------------
/// @brief      Right shift operator overload, for higher verbosity in input stream operations.
///
/// @param      istream    Stream to read data from.
/// @param[in]  range      Target Sequence::Range (or sequence) instance.
///
/// @return     Modified/consumed input stream.
///
/// @note       Wraps around read() wo/ additional delimiter character selection.
///
std::istream& operator>>(std::istream& istream, trigno::Sequence::Range range);

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_IO_HPP_
