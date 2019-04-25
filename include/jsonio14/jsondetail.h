#pragma once

#include <string>
#include <sstream>
#include <limits>
#include <queue>
//#include "type_test.h"

namespace jsonio14 {


/// Basic class for read/write scalar/array to format
struct DetailSettings
{
    static int doublePrecision;	///< Precision of double
    static int floatPrecision;	///< Precision of float
};


/// Read value from string
template <typename T>
bool is( T& x, const std::string& s)
{
  std::istringstream iss(s);
  return iss >> x && !iss.ignore();
}


/// Converts a numeric value to a string.
template < typename T >
std::string v2string( const T& value )
{
    return std::to_string(value);
}

/// Converts a string to a string.
template <> std::string v2string( const std::string& value );
/// Converts a numeric value to a string.
std::string v2string( const char* value );

/// Deserialization from string.
template <class T>
bool string2v( const std::string& data, T& value )
{
    if( data.find("null") != std::string::npos )
    {
        value = std::numeric_limits<T>::min();
        return true;
    }
    std::istringstream iss(data);
    return iss >> value && !iss.ignore();
}
template <>  bool string2v( const std::string& data, std::string& value );
template <>  bool string2v( const std::string& data, bool& value );


// Some string functions -----------------------------------------------------


/// Split string to int array ( "1;2;3" to  { 1, 2, 3 } )
std::queue<int> split_int( const std::string& str_data, const std::string& delimiters );
/// A split string function  ( "a;b;c" to  { "a", "b", "c" } )
std::queue<std::string> split(const std::string& str, const std::string& delimiters);

} // namespace jsonio14
