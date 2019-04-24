#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

namespace jsonio14 {


///  Function that can be used to split text using regexp
std::vector<std::string> regexp_split(const std::string& str, std::string rgx_str = "\\s+");
///  Function that can be used to extract tokens using regexp
std::vector<std::string> regexp_extract(const std::string& str, std::string rgx_str);
///  Function that can be used to replase text using regexp
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement );

/// Trim from start (in place)
inline void ltrim(std::string &s )
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

/// Trim from end (in place)
inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

/// Trim from both ends (in place)
inline void trim(std::string &s )
{
    ltrim(s);
    rtrim(s);
}

/// Trim characters from start (in place)
inline void ltrim(std::string &s, const std::string &characters )
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [=](char ch) {
        return characters.find_first_of(ch)==std::string::npos;
    }));
}

/// Trim characters from end (in place)
inline void rtrim(std::string &s, const std::string &characters)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [=](char ch) {
        return characters.find_first_of(ch)==std::string::npos;
    }).base(), s.end());
}

/// Trim characters from both ends (in place)
inline void trim(std::string &s, const std::string &characters )
{
    ltrim(s, characters);
    rtrim(s, characters);
}


/// Replace all characters to character in string (in place)
inline void replace_all(std::string &s, const std::string &characters, char to_character )
{
    std::replace_if( s.begin(), s.end(), [=](char ch) {
        return characters.find_first_of(ch)!=std::string::npos;
    }, to_character );
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


} // namespace jsonio14
