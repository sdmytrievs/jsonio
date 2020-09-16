#pragma once

#include "jsonio17/jsonbase.h"


namespace jsonio17 {

class JsonObjectBuilder;
class JsonArrayBuilder;

///  Base interface for builder of JsonBase object.
class JsonBuilderBase
{

public:

    /// Destructor
    virtual ~JsonBuilderBase()
    {}

    /// Adds a key/ empty Json object pair to the JSON object associated with this object builder.
    virtual JsonObjectBuilder addObject( const std::string& key )  = 0;
    /// Adds a key/ empty Json array pair to the JSON object associated with this object builder.
    virtual JsonArrayBuilder  addArray( const std::string& key )  = 0;
    /// Adds a key/ string value pair to the JSON object associated with this object builder.
    virtual JsonBuilderBase& addString( const std::string& key, const std::string& value ) =0;
    /// Adds a key/ value pair to the JSON object associated with this object builder.
    /// Infers the value type by parsing input string. Throw Exeption when undefined value type.
    virtual JsonBuilderBase& testScalar(const std::string& key, const std::string& value, bool no_string=false );

    /*template <class T>
    operator T() const
    {
        return static_cast<T>(current_json);
    }*/

protected:

    /// Internal JSON object
    JsonBase*  current_json;

    /// Constructor
    explicit JsonBuilderBase( JsonBase* object )
        :current_json{object}
    { }

    /// Infers the value type by parsing input string.
    void append_scalar(const std::string &key, const std::string &value, bool noString );

};

///  A builder for creating JsonBase::Object from scratch.
///
///  This class initializes an empty JSON object and provides methods to add name/value pairs to the object.
///  The methods in this class can be chained to add multiple name/value pairs to the object.
class JsonObjectBuilder final : public JsonBuilderBase
{

public:

    /// Constructor
    explicit JsonObjectBuilder( JsonBase* object )
        : JsonBuilderBase{ object }
    {
        // clear old data
        if( object )
           object->update_node( JsonBase::Object, "" );
    }

    /// Adds a key/ empty Json object pair to the JSON object associated with this object builder.
    JsonObjectBuilder addObject( const std::string& akey ) override;

    /// Adds a key/ empty Json array pair to the JSON object associated with this object builder.
    JsonArrayBuilder  addArray( const std::string& akey ) override;

    /// Adds a key/ null object pair to the JSON object associated with this object builder.
    JsonObjectBuilder& addNull( const std::string& akey )
    {
        if( current_json )
            current_json->append_node( akey, JsonBase::Null, "null" );
        return *this;
    }

    /// Adds a key/ boolean value pair to the JSON object associated with this object builder.
    JsonObjectBuilder& addBool( const std::string& akey, bool value )
    {
        if( current_json )
            current_json->append_node( akey, JsonBase::Bool, v2string(value) );
        return *this;
    }

    /// Adds a key/ integer value pair to the JSON object associated with this object builder.
    JsonObjectBuilder& addInt( const std::string& akey, long value )
    {
        if( current_json )
            current_json->append_node( akey, JsonBase::Int, v2string(value) );
        return *this;
    }

    /// Adds a key/ float point value pair to the JSON object associated with this object builder.
    JsonObjectBuilder& addDouble( const std::string& akey, double value )
    {
        if( current_json )
            current_json->append_node( akey, JsonBase::Double, v2string(value) );
        return *this;
    }

    /// Adds a key/ string value pair to the JSON object associated with this object builder.
    JsonObjectBuilder& addString( const std::string& akey, const std::string& value ) override
    {
        if( current_json )
            current_json->append_node( akey, JsonBase::String, v2string(value) );
        return *this;
    }

    /// Adds a key/ a primitive type value pair to the JSON object associated with this object builder.
    /// Infers the value type by parsing input string.
    JsonObjectBuilder& addScalar(const std::string& key, const std::string& value )
    {
        append_scalar( key,  value, false  );
        return *this;
    }

    /// Adds a key/ a primitive type value  pair to the JSON object associated with this object builder.
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    JsonObjectBuilder&  addValue( const std::string& akey, T value )
    {
        if( current_json )
        {
            auto decodedType = current_json->typeTraits( value );
            current_json->append_node( akey, decodedType, v2string(value) );
        }
        return *this;
    }

    /// Adds a key/ vector-like object pair to the JSON object associated with this object builder.
    /// Vector-like objects: std::list, std::vector, std::set, etc.
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    JsonObjectBuilder&  addVector( const std::string& akey, const T& values  )
    {
        if( current_json )
        {
            auto obj = current_json->append_node( akey, JsonBase::Array, "" );
            if( obj )
                obj->set_list_from( values  );
        }
        return *this;
    }

    /// Adds a key/ map-like object pair to the JSON object associated with this object builder.
    /// Map-like objects: std::map, std::unordered_map, etc.
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    JsonObjectBuilder&  addMapKey( const std::string& akey, const T& values  )
    {
        if( current_json )
        {
            auto obj = current_json->append_node( akey, JsonBase::Object, "" );
            if( obj )
                obj->set_map_from( values  );
        }
        return *this;
    }

};

/// A builder for creating JsonBase::Array from scratch.
///
/// This class initializes an empty JSON array and provides methods to add values to the array.
/// The methods in this class can be chained to add multiple values to the array.
class JsonArrayBuilder final : public JsonBuilderBase
{

public:

    /// Constructor
    explicit JsonArrayBuilder( JsonBase* object )
        : JsonBuilderBase{ object }
    {
        // clear old data
        if( object )
            object->update_node( JsonBase::Array, "" );
    }

    /// Adds an empty Json object to the JSON array associated with this object builder.
    JsonObjectBuilder addObject();

    /// Adds an empty Json array to the JSON array associated with this object builder.
    JsonArrayBuilder  addArray();


    /// Adds a null object to the JSON array associated with this object builder.
    JsonArrayBuilder& addNull()
    {
        if( current_json )
            current_json->append_node( nextKey(), JsonBase::Null, "null" );
        return *this;
    }

    /// Adds a boolean value to the JSON array associated with this object builder.
    JsonArrayBuilder& addBool( bool value )
    {
        if( current_json )
            current_json->append_node( nextKey(), JsonBase::Bool, v2string(value) );
        return *this;
    }

    /// Adds an integer value to the JSON array associated with this object builder.
    JsonArrayBuilder& addInt( long value )
    {
        if( current_json )
            current_json->append_node( nextKey(), JsonBase::Int, v2string(value) );
        return *this;
    }

    /// Adds a double value to the JSON array associated with this object builder.
    JsonArrayBuilder& addDouble( double value )
    {
        if( current_json )
            current_json->append_node( nextKey(), JsonBase::Double, v2string(value) );
        return *this;
    }

    /// Adds a string value to the JSON array associated with this object builder.
    JsonArrayBuilder& addString( const std::string& value )
    {
        if( current_json )
            current_json->append_node( nextKey(), JsonBase::String, v2string(value) );
        return *this;
    }

    /// Adds a primitive type value to the JSON array associated with this object builder.
    /// Infers the value type by parsing input string.
    JsonArrayBuilder& addScalar( const std::string& value )
    {
        append_scalar( nextKey(),  value, false  );
        return *this;
    }

    /// Adds a primitive type value to the JSON array associated with this object builder.
    /// A primitive type value: string, number, boolean, etc.
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    JsonArrayBuilder&  addValue( T value )
    {
        if( current_json )
        {
            auto decodedType = current_json->typeTraits( value );
            current_json->append_node( nextKey(), decodedType, v2string(value) );
        }
        return *this;
    }

    /// Adds a vector-like object to the JSON array associated with this object builder.
    /// Vector-like objects: std::list, std::vector, std::set, etc.
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    JsonArrayBuilder&  addVector( const T& values  )
    {
        if( current_json )
        {
            auto obj = current_json->append_node( nextKey(), JsonBase::Array, "" );
            if( obj )
                obj->set_list_from( values  );
        }
        return *this;
    }

    /// Adds a map-like object to the JSON array associated with this object builder.
    /// Map-like objects: std::map, std::unordered_map, etc.
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    JsonArrayBuilder&  addMapKey( const T& values  )
    {
        if( current_json )
        {
            auto obj = current_json->append_node( nextKey(), JsonBase::Object, "" );
            if( obj )
                obj->set_map_from( values  );
        }
        return *this;
    }

    // Addition functions with test key

    /// Adds an empty Json object to the JSON array associated with this object builder.
    /// With testing that the key is correct.
    JsonObjectBuilder addObject( const std::string& akey ) override;
    /// Adds an empty Json array to the JSON array associated with this object builder.
    /// With testing that the key is correct.
    JsonArrayBuilder  addArray( const std::string& akey ) override;
    /// Adds a string value to the JSON array associated with this object builder.
    /// With testing that the key is correct.
    JsonBuilderBase &addString( const std::string& akey, const std::string& value ) override;

    /// Foresee the next key.
    std::string nextKey() const
    {
        if(current_json)
            return std::to_string( current_json->getChildrenCount() );
        else
            return "0";
    }
};

} // namespace jsonio17
