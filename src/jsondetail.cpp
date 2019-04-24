#include <cmath>
#include "jsondetail.h"
#include "service.h"

namespace jsonio14 {

int DetailSettings::doublePrecision = 15;
int DetailSettings::floatPrecision = 7;

/// Serialization string value to string
template <> std::string v2string( const std::string& value )
{
    return value;
}

/// Serialization double value to string
template <> std::string v2string( const double& value )
{
    if(std::isfinite(value))
    {
        char vbuf[50];
        snprintf(vbuf, sizeof vbuf, "%.*lg" , DetailSettings::doublePrecision, value );
        return vbuf;
    } else
    {
        return"null";
    }
}

/// Serialization float value to string
template <> std::string v2string( const float& value )
{
    if(std::isfinite(value))
    {
        char vbuf[50];
        snprintf(vbuf, sizeof vbuf, "%.*g" , DetailSettings::floatPrecision, value );
        return vbuf;
    } else
    {
        return"null";
    }
}

/// Serialization char value to string
template<> std::string v2string( const char& value )
{
    return std::string(1, value);
}

/// Serialization char value to string
template<> std::string v2string( const unsigned char& value )
{
    return std::string(1, static_cast<const char>(value));
}

/// Serialization bool value to string
template<> std::string v2string( const bool& value )
{
    return value ? "true" : "false";;
}

std::string v2string( const char* value )
{
    return std::string(value);
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


// Serialization part --------------------------------------------------


std::string dump(const std::string &value )
{
    std::string out;
    out += '"';
    for (size_t i = 0; i < value.length(); i++) {
        const char ch = value[i];
        if (ch == '\\') {
            out += "\\\\";
        } else if (ch == '"') {
            out += "\\\"";
        } else if (ch == '\b') {
            out += "\\b";
        } else if (ch == '\f') {
            out += "\\f";
        } else if (ch == '\n') {
            out += "\\n";
        } else if (ch == '\r') {
            out += "\\r";
        } else if (ch == '\t') {
            out += "\\t";
        } else if (static_cast<uint8_t>(ch) <= 0x1f) {
            char buf[8];
            snprintf(buf, sizeof buf, "\\u%04x", ch);
            out += buf;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa8) {
            out += "\\u2028";
            i += 2;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa9) {
            out += "\\u2029";
            i += 2;
        } else {
            out += ch;
        }
    }
    out += '"';

    return out;
}

std::string dump(const char *value )
{
    return dump( std::string(value) );
}

// "1;2;3" to array { 1, 2, 3 }
std::queue<int> split_int( const std::string& str_data, const std::string& delimiters )
{
    std::queue<int> res;
    int value;

    if( str_data.empty() )
        return res;

    std::string str = str_data;

    size_t pos = str.find( delimiters );
    while( pos != std::string::npos )
    {
        auto part = str.substr(0, pos);
        if( is<int>( value, str.substr(0, pos) ) )
            res.push( value );
        str = str.substr( pos + delimiters.length() );
        pos = str.find( delimiters );
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
        //if(pos != start) // ignore empty tokens
        {
            auto vv = std::string(str, start, pos - start);
            trim(vv);
            v.push( vv );
        }
        start = pos + 1;
        pos = str.find_first_of(delimiters, start);
    }
    //if(start < str.length()) // ignore trailing delimiter
    v.push( std::string (str, start, str.length() - start) );
    return v;
}


} // namespace jsonio14
