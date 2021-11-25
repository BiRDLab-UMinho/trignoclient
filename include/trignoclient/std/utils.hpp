#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_UTILS_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_UTILS_HPP_

#include <string>
#include <vector>

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
inline C tokenize(const string& data, const string separator = ",", bool single_split = false) {
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

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_UTILS_HPP_
