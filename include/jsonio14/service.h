#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <limits>

namespace jsonio14 {


///  Function that can be used to split text using regexp
std::vector<std::string> regexp_split(const std::string& str, std::string rgx_str = "\\s+");
///  Function that can be used to extract tokens using regexp
std::vector<std::string> regexp_extract(const std::string& str, std::string rgx_str);
///  Function that can be used to replase text using regexp
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement );


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
