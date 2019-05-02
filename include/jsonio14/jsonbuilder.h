#pragma once

#include "jsonbase.h"

namespace jsonio14 {

class JsonFree;
class JsonSchema;

namespace json {

/// @brief dump object to JSON string.
std::string dump( const JsonBase& object, bool dense = false );

/// Serialize object as a JSON formatted stream.
void dump( std::ostream& os, const JsonBase& object, bool dense = false );

/// Deserialize a JSON document to a object.
void loads( const std::string& jsonstr, JsonBase& object );

/// Deserialize a JSON document to a free format json object.
JsonFree loads( const std::string& jsonstr );
/// Deserialize a JSON document to a schema define json object.
JsonSchema loads( const std::string& schema_name, const std::string& jsonstr );


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
    genjson.pop_back();
    genjson += " }";
    return genjson;
}

} // namespace jsonio14

class JsonObjectBuilder;
class JsonArrayBuilder;

class JsonBuilderBase
{

protected:

    JsonBase&  current_json;

    explicit JsonBuilderBase( JsonBase& object )
        :current_json{object}
    { }

    void append_scalar(const std::string &key, const std::string &value, bool noString );

public:

    virtual ~JsonBuilderBase()
    {}

    virtual JsonObjectBuilder addObject( const std::string& akey )  = 0;
    virtual JsonArrayBuilder  addArray( const std::string& akey )  = 0;
    virtual JsonBuilderBase& addString( const std::string& akey, const std::string& value ) =0;
    virtual JsonBuilderBase& testScalar(const std::string& key, const std::string& value )
    {
        append_scalar( key,  value, true  );
        return *this;
    }

    /*template <class T>
    operator T() const
    {
        return static_cast<T>(current_json);
    }*/

};

class JsonObjectBuilder final : public JsonBuilderBase
{

public:

    explicit JsonObjectBuilder( JsonBase& object )
        : JsonBuilderBase{ object }
    {
        // clear old data
        object.update_node( JsonBase::Object, "" );
    }

    // Append functions --------------------------------------

    JsonObjectBuilder addObject( const std::string& akey ) override;

    JsonArrayBuilder  addArray( const std::string& akey ) override;

    JsonObjectBuilder& addNull( const std::string& akey )
    {
        current_json.append_node( akey, JsonBase::Null, "null" );
        return *this;
    }

    JsonObjectBuilder& addBool( const std::string& akey, bool value )
    {
        current_json.append_node( akey, JsonBase::Bool, v2string(value) );
        return *this;
    }

    JsonObjectBuilder& addInt( const std::string& akey, long value )
    {
        current_json.append_node( akey, JsonBase::Int, v2string(value) );
        return *this;
    }

    JsonObjectBuilder& addDouble( const std::string& akey, double value )
    {
        current_json.append_node( akey, JsonBase::Double, v2string(value) );
        return *this;
    }

    JsonObjectBuilder& addString( const std::string& akey, const std::string& value ) override
    {
        current_json.append_node( akey, JsonBase::String, v2string(value) );
        return *this;
    }

    /// Append obvious object - get type from string data
    JsonObjectBuilder& addScalar(const std::string& key, const std::string& value )
    {
        append_scalar( key,  value, false  );
        return *this;
    }

    /// Add primitive type value (string, number, boolean ).
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    JsonObjectBuilder&  addValue( const std::string& akey, T value )
    {
        auto decodedType = current_json.typeTraits( value );
        current_json.append_node( akey, decodedType, v2string(value) );
        return *this;
    }

    /// Add vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    JsonObjectBuilder&  addVector( const std::string& akey, const T& values  )
    {
        current_json.append_node( akey, JsonBase::Array, "" ).setArray( values  );
        return *this;
    }

    /// Add map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    JsonObjectBuilder&  addMapKey( const std::string& akey, const T& values  )
    {
        current_json.append_node( akey, JsonBase::Object, "" ).setMapKey( values  );
        return *this;
    }

};

class JsonArrayBuilder final : public JsonBuilderBase
{

public:

    explicit JsonArrayBuilder( JsonBase& object )
        : JsonBuilderBase{ object }
    {
        // clear old data
        object.update_node( JsonBase::Array, "" );
    }

    // Append functions --------------------------------------

    JsonObjectBuilder addObject();

    JsonArrayBuilder  addArray();

    JsonArrayBuilder& addNull()
    {
        current_json.append_node( nextKey(), JsonBase::Null, "null" );
        return *this;
    }

    JsonArrayBuilder& addBool( bool value )
    {
        current_json.append_node( nextKey(), JsonBase::Bool, v2string(value) );
        return *this;
    }

    JsonArrayBuilder& addInt( long value )
    {
        current_json.append_node( nextKey(), JsonBase::Int, v2string(value) );
        return *this;
    }

    JsonArrayBuilder& addDouble( double value )
    {
        current_json.append_node( nextKey(), JsonBase::Double, v2string(value) );
        return *this;
    }

    JsonArrayBuilder& addString( const std::string& value )
    {
        current_json.append_node( nextKey(), JsonBase::String, v2string(value) );
        return *this;
    }

    /// Append obvious object - get type from string data
    JsonArrayBuilder& addScalar( const std::string& value )
    {
        append_scalar( nextKey(),  value, false  );
        return *this;
    }

    /// Add primitive type value (string, number, boolean ).
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    JsonArrayBuilder&  addValue( T value )
    {
        auto decodedType = current_json.typeTraits( value );
        current_json.append_node( nextKey(), decodedType, v2string(value) );
        return *this;
    }

    /// Add vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    JsonArrayBuilder&  addVector( const T& values  )
    {
        current_json.append_node( nextKey(), JsonBase::Array, "" ).setArray( values  );
        return *this;
    }

    /// Add map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    JsonArrayBuilder&  addMapKey( const T& values  )
    {
        current_json.append_node( nextKey(), JsonBase::Object, "" ).setMapKey( values  );
        return *this;
    }

    // Addition functions with test key

    JsonObjectBuilder addObject( const std::string& akey ) override;
    JsonArrayBuilder  addArray( const std::string& akey ) override;
    JsonBuilderBase &addString( const std::string& akey, const std::string& value ) override;

    std::string nextKey() const
    {
      return  std::to_string( current_json.getChildrenCount() );
    }
};



} // namespace jsonio14
