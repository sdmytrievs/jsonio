#pragma once

#include "jsonfree.h"

namespace jsonio14 {


//template <class T>
class JsonBuilder final
{
protected:

    JsonFree  root_json;
    JsonFree& current_json;

    explicit JsonBuilder( const std::string root_name )
        : root_json{ JsonBase::Object, root_name, "" }, current_json(root_json)
    { }

public:

    operator JsonFree() const
    {
        return std::move(root_json);
    }

    // Append functions --------------------------------------

    JsonBuilder& startObject( const std::string& akey )
    {
        current_json.append_node( akey, JsonBase::Object, "" );
        current_json = current_json.children.back();
        return *this;
    }

    JsonBuilder& endObject()
    {
        current_json = current_json.get_parent();
        return *this;
    }

    JsonBuilder& startArray( const std::string& akey )
    {
        current_json.append_node( akey, JsonBase::Array, "" );
        current_json = current_json.children.back();
        return *this;
    }

    JsonBuilder& endArray()
    {
        current_json = current_json.get_parent();
        return *this;
    }

    JsonBuilder& addNull( const std::string& akey )
    {
        current_json.append_node( akey, JsonBase::Null, "null" );
        return *this;
    }

    JsonBuilder& addBool( const std::string& akey, bool value )
    {
        current_json.append_node( akey, JsonBase::Bool, v2string(value) );
        return *this;
    }

    JsonBuilder& addInt( const std::string& akey, long value )
    {
        current_json.append_node( akey, JsonBase::Null, v2string(value) );
        return *this;
    }

    JsonBuilder& addDouble( const std::string& akey, double value )
    {
        current_json.append_node( akey, JsonBase::Null, v2string(value) );
        return *this;
    }

    JsonBuilder& addString( const std::string& akey, const std::string& value )
    {
        current_json.append_node( akey, JsonBase::Null, v2string(value) );
        return *this;
    }

    /// Append obvious object - get type from string data
    JsonBuilder& addScalar(const std::string& key, const std::string& value );


    /// Add primitive type value (string, number, boolean ).
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    JsonBuilder&  addValue( const std::string& akey, T value )
    {
        auto decodedType = current_json.typeTraits( value );
        current_json.append_node( akey, decodedType, v2string(value) );
        return *this;
    }

    /// Add vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    JsonBuilder&  addArray( const std::string& akey, const T& values  )
    {
        current_json.append_node( akey, JsonBase::Array, "" ).setArray( values  );
        return *this;
    }

    /// Add map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    JsonBuilder&  addMapKey( const std::string& akey, const T& values  )
    {
        current_json.append_node( akey, JsonBase::Object, "" ).setMapKey( values  );
        return *this;
    }

};


} // namespace jsonio14
