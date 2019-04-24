#pragma once

#include "jsonbase.h"

namespace jsonio14 {

class JsonObjectBuilder;
class JsonArrayBuilder;

class JsonBuilderBase
{

protected:

    JsonBase&  current_json;

    explicit JsonBuilderBase( JsonBase& object )
        :current_json{object}
    { }

    void append_scalar(const std::string &key, const std::string &value);

public:

    virtual JsonObjectBuilder addObject( const std::string& akey ) = 0;
    virtual JsonArrayBuilder  addArray( const std::string& akey ) = 0;
    virtual JsonBuilderBase& addString( const std::string& akey, const std::string& value ) =0;
    virtual JsonBuilderBase& addScalar(const std::string& key, const std::string& value ) =0;

    /*template <class T>
    operator T() const
    {
        return static_cast<T>(current_json);
    }*/

};


//template <class T>
class JsonObjectBuilder final : public JsonBuilderBase
{

public:

    explicit JsonObjectBuilder( JsonBase& object )
        : JsonBuilderBase{ object }
    { }

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
    JsonObjectBuilder& addScalar(const std::string& key, const std::string& value ) override
    {
        append_scalar( key,  value  );
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

//template <class T>
class JsonArrayBuilder final : public JsonBuilderBase
{


public:

    explicit JsonArrayBuilder( JsonBase& object )
        : JsonBuilderBase{ object }
    { }

    // Append functions --------------------------------------

    JsonObjectBuilder addObject();

    JsonArrayBuilder  addArray();

    JsonArrayBuilder& addNull()
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Null, "null" );
        return *this;
    }

    JsonArrayBuilder& addBool( bool value )
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Bool, v2string(value) );
        return *this;
    }

    JsonArrayBuilder& addInt( long value )
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Int, v2string(value) );
        return *this;
    }

    JsonArrayBuilder& addDouble( double value )
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Double, v2string(value) );
        return *this;
    }

    JsonArrayBuilder& addString( const std::string& value )
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::String, v2string(value) );
        return *this;
    }

    /// Append obvious object - get type from string data
    JsonArrayBuilder& addScalar( const std::string& value )
    {
        append_scalar( std::to_string( current_json.getChildrenCount() ),  value  );
        return *this;
    }

    /// Add primitive type value (string, number, boolean ).
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    JsonArrayBuilder&  addValue( T value )
    {
        auto decodedType = current_json.typeTraits( value );
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), decodedType, v2string(value) );
        return *this;
    }

    /// Add vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    JsonArrayBuilder&  addVector( const T& values  )
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Array, "" ).setArray( values  );
        return *this;
    }

    /// Add map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    JsonArrayBuilder&  addMapKey( const T& values  )
    {
        current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Object, "" ).setMapKey( values  );
        return *this;
    }

    // Addition functions with test key

    JsonObjectBuilder addObject( const std::string& akey ) override;
    JsonArrayBuilder  addArray( const std::string& akey ) override;
    JsonBuilderBase &addString( const std::string& akey, const std::string& value ) override;
    JsonBuilderBase &addScalar(const std::string& key, const std::string& value ) override;

};



} // namespace jsonio14
