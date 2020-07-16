#include <iomanip>
#include "jsonio17/jsondetail.h"
#include "jsonio17/service.h"
#include "jsonio17/exceptions.h"

namespace jsonio17 {

int DetailSettings::doublePrecision = 15;
int DetailSettings::floatPrecision = 7;
const char* DetailSettings::infiniteValue = "null";


std::string v2string( const char* value )
{
    return std::string(value);
}

template <> std::string v2string( const std::string& value )
{
    return value;
}

// Serialization bool value to string.
template<> std::string v2string( const bool& value )
{
    return value ? "true" : "false";
}

// Serializations double value to string.
template <> std::string v2string( const double& value )
{
    if(std::isfinite(value))
    {
        std::ostringstream os;
        os << std::setprecision(DetailSettings::doublePrecision) << value;
        return os.str();
    } else
    {
        return DetailSettings::infiniteValue;
    }
}

// Serializations float value to string.
template <> std::string v2string( const float& value )
{
    if(std::isfinite(value))
    {
        std::ostringstream os;
        os << std::setprecision(DetailSettings::floatPrecision) << value;
        return os.str();
    } else
    {
        return DetailSettings::infiniteValue;
    }
}

template <>  bool string2v( const std::string& data, std::string& value )
{
    value = data;
    return true;
}

template <>  bool string2v( const std::string& data, bool& value )
{
    value = ( data.find("true") != std::string::npos ? true: false);
    return true;
}

// "1;2;3" to array { 1, 2, 3 }
std::queue<int> split_int( const std::string& str_data, const std::string& delimiter )
{
    std::queue<int> res;
    int value;

    if( str_data.empty() )
        return res;

    std::string str = str_data;

    size_t pos = str.find( delimiter );
    while( pos != std::string::npos )
    {
        auto part = str.substr(0, pos);
        if( is<int>( value, str.substr(0, pos) ) ){
            res.push( value );
        }
        else {
          JSONIO_THROW( "detail", 1, "illegal input value " + str_data );
        }
        str = str.substr( pos + delimiter.length() );
        pos = str.find( delimiter );
    }
    if( is<int>( value, str ) )
        res.push( value );
    return res;
}

// "a;b;c" to array { "a", "b", "c" }
std::queue<std::string> split(const std::string& str, const std::string& delimiters)
{
    std::queue<std::string> v;

    if( str.empty() )
        return v;

    std::string::size_type start = 0;
    auto pos = str.find_first_of(delimiters, start);
    while(pos != std::string::npos)
    {
        if(pos != start) // ignore empty tokens
        {
            auto vv = std::string(str, start, pos - start);
            trim(vv);
            v.push( vv );
        }
        start = pos + 1;
        pos = str.find_first_of(delimiters, start);
    }
    if(start < str.length()) // ignore trailing delimiter
        v.push( std::string (str, start, str.length() - start) );
    return v;
}


} // namespace jsonio17
