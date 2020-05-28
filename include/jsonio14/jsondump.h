#pragma once

#include "jsonio14/jsonbase.h"

namespace jsonio14 {

class JsonFree;
class JsonSchema;

namespace json {


/// @brief Dump object to JSON string.
std::string dump( const JsonBase& object, bool dense = false );

/// Serialize object as a JSON formatted stream.
void dump( std::ostream& os, const JsonBase& object, bool dense = false );

/// Deserialize a JSON string to a object.
void loads( const std::string& jsonstr, JsonBase& object );


/// Deserialize a JSON document to a free format json object.
JsonFree loads( const std::string& jsonstr );
/// Deserialize a JSON document to a schema define json object.
JsonSchema loads( const std::string& aschema_name, const std::string& jsonstr );


/// @brief dump escaped string.
/// Escape a string by replacing certain special characters by a sequence of an
/// escape character (backslash) and another character and other control.
std::string dump( const std::string& value );
/// @brief dump escaped string.
/// Escape a string by replacing certain special characters by a sequence of an
/// escape character (backslash) and another character and other control.
std::string dump( const char* value );

/// Serialize a numeric value  to a json string.
template <class T,
          std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
std::string dump( const T& value  )
{
    return v2string(value);
}

/// Serialize vector-like objects (std::list, std::vector, std::set, etc) to a json string.
template <class T,
          std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
std::string dump( const T& elems  )
{
    std::string genjson = "[";
    for( auto row: elems )
    {
        genjson += " "+dump( row ) + ",";
    }
    if( !elems.empty() )
        genjson.pop_back();
    genjson += " ]";
    return genjson;
}

/// Serialize map-like objects (std::map, std::unordered_map, etc) to a json string.
template <class T,
          std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
std::string dump( const T& elems  )
{
    std::string genjson = "{";
    for( auto row: elems )
    {
        genjson += " \"" + row.first + "\"";
        genjson += ":" + dump(row.second) + ",";
    }
    if( !elems.empty() )
        genjson.pop_back();
    genjson += " }";
    return genjson;
}

///  @brief Scan a string literal.
///  Back to dump escaped string.
void undumpString( std::string& strvalue );

} // namespace json

} // namespace jsonio14

