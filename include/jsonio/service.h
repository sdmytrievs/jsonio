#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cwctype>

namespace jsonio {


///  Function that can be used to split text using regexp.
std::vector<std::string> regexp_split(const std::string& str, std::string rgx_str = "\\s+");
///  Function that can be used to extract tokens using regexp.
std::vector<std::string> regexp_extract(const std::string& str, std::string rgx_str);
///  Function that can be used to replace text using regex.
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement );
///  Returns true whether the string matches the regular expression.
bool regexp_test(const std::string& str, std::string rgx_str);
///  Function that can be used to replace text.
std::string string_replace_all(const std::string& instr, const std::string& replace_from, const std::string& replace_to);

//// Extract the string value from data.
std::string regexp_extract_string( const std::string& regstr, const std::string& data );
/// Extract the string value by key from json string
std::string extract_string_json( const std::string& key, const std::string& jsondata );
/// Extract the int value by key from json string
int extract_int_json( const std::string& key, const std::string& jsondata );

/// Replace all characters to character in string (in place).
inline void replace_all(std::string &s, const std::string &characters, char to_character )
{
    std::replace_if( s.begin(), s.end(), [=](char ch) {
        return characters.find_first_of(ch)!=std::string::npos;
    }, to_character );
}

template < class T>
inline bool in_range( const T& x, const T& lower, const T& upper)
{
    return (x >= lower && x <= upper);
}


/// Trim all whitespace characters from start (in place).
inline void ltrim(std::string &s )
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !isspace(ch);
    }));
}

/// Trim all whitespace characters from end (in place).
inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !isspace(ch);
    }).base(), s.end());
}

/// Trim all whitespace characters from both ends (in place).
inline void trim(std::string &s )
{
    ltrim(s);
    rtrim(s);
}

/// Trim characters from start (in place).
inline void ltrim(std::string &s, const std::string &characters )
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [=](char ch) {
        return characters.find_first_of(ch)==std::string::npos;
    }));
}

/// Trim characters from end (in place).
inline void rtrim(std::string &s, const std::string &characters)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [=](char ch) {
        return characters.find_first_of(ch)==std::string::npos;
    }).base(), s.end());
}

/// Trim characters from both ends (in place).
inline void trim(std::string &s, const std::string &characters )
{
    ltrim(s, characters);
    rtrim(s, characters);
}

template<typename T>
bool approximatelyEqual( const T& a, const T& b, const T& epsilon = std::numeric_limits<T>::epsilon() )
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

template<typename T>
bool essentiallyEqual( const T& a, const T& b, const T& epsilon = std::numeric_limits<T>::epsilon() )
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

template<typename T>
bool definitelyGreaterThan( const T& a, const T& b, const T& epsilon = std::numeric_limits<T>::epsilon() )
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

template<typename T>
bool definitelyLessThan( const T& a, const T& b, const T& epsilon = std::numeric_limits<T>::epsilon() )
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

/// Generate vector with minimal values from two vectors
std::vector<size_t> min_vector(const std::vector<size_t>& lhs, const std::vector<size_t>& rhs);

} // namespace jsonio
