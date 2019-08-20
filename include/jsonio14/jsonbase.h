#pragma once

#include <iostream>
#include <vector>
#include "jsondetail.h"
#include "exceptions.h"
#include "type_test.h"

namespace jsonio14 {


/// @brief An interface to store JSON object.
/// @class JsonBase represents an abstract item in a tree view.
/// Item data defines one json object.
class JsonBase
{
protected:

    explicit JsonBase() {}
    /// Destructor
    virtual ~JsonBase() {}

public:

    enum Type {
        UNDEFINED = -1,
        Null = 1,
        Bool = 2,    // T_BOOL
        Int = 8,     // T_I32
        Double = 4,  // T_DOUBLE
        String = 11, // T_STRING
        Object = 12, // T_STRUCT
        Array = 15   // T_LIST
    };

    // Test methods  --------------------------------------------

    /// This function returns true if and only if the JSON object is top.
    virtual bool isTop() const = 0;

    /// This function returns true if and only if the JSON type is Object.
    virtual bool isObject() const
    { return type() == Type::Object; }

    /// This function returns true if and only if the JSON type is Array.
    virtual bool isArray() const
    { return  type() == Type::Array; }

    /// This function returns true if and only if the JSON type is a numeric value.
    virtual bool isNumber( ) const
    { return  type() == Type::Int or type() == Type::Double; }

    /// This function returns true if and only if the JSON type is boolean.
    virtual bool isBool() const
    { return type() == Type::Bool; }

    /// This function returns true if and only if the JSON type is Null object.
    virtual bool isNull() const
    { return type() == Type::Null; }

    /// This function returns true if and only if the JSON type is String
    virtual bool isString() const
    { return type() == Type::String; }

    /// @brief return whether type is primitive
    /// This function returns true if and only if the JSON type is primitive
    /// (string, number, boolean, or null).
    bool isPrimitive() const
    {
        return isNull() or isString() or isBool() or isNumber();
    }

    /// @brief return whether type is structured
    /// This function returns true if and only if the JSON type is structured
    /// (array or object).
    bool isStructured() const
    {
        return isArray() or isObject();
    }

    // Get methods  --------------------------

    /// Returns a std::string with the json contained in this object.
    virtual std::string toString(bool dense = false) const;
    /// Returns the object converted to a double value.
    virtual double toDouble() const;
    /// Returns the object converted to a long value.
    virtual long   toInt() const;
    /// Returns the object converted to a boolean value.
    virtual bool   toBool() const;

    /// Get object type.
    virtual Type type() const = 0;

    /// @brief return the type as string
    ///   Returns the type name as string to be used in error messages - usually to
    ///   indicate that a function was called on a wrong JSON type.
    virtual const char* typeName() const
    {
        return  typeName( type() );
    }

    std::size_t size() const
    {
      return  getChildrenCount();
    }

    bool empty() const
    {
      return size() < 1;
    }

    // Get methods ( using in Qt GUI ) --------------------------

    virtual const std::string& getKey() const = 0;

    virtual size_t getNdx() const = 0;

    virtual const std::string& getFieldValue() const = 0;

    virtual std::size_t getChildrenCount() const = 0;

    virtual const JsonBase* getChild( std::size_t ndx ) const = 0;

    virtual const JsonBase* getParent() const = 0;

    virtual std::vector<std::string> getUsedKeys() const = 0;

    /// Get object name.
    virtual std::string getHelpName() const;

    /// Get object description.
    virtual std::string getDescription() const
    {
        return getHelpName();
    }

    /// Get field path to top object.
    std::string getFieldPath() const;

    // Get values  --------------------------

    bool get_to( std::string& value  ) const
    {
        value = toString(true);
        return true;
    }

    /// Get primitive value from current Node
    /// Could be get_to( T& value ) function
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        auto decodedType = typeTraits( value );
        if( decodedType>= Null && decodedType<=String )
        {
            return string2v( getFieldValue(), value );
        }
        return false;
    }

    /// Get array-like data from current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        return get_to_list( value );
    }

    /// Get map-like data from current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        return get_to_map( value );
    }

    /// Get  vector-like objects (std::list, std::vector ) from current Node
    /// Container must have emplace_back function
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    bool get_to_list( T& values  ) const
    {
        JARANGO_THROW_IF( !isArray(), "JsonBase", 11, "cannot use getArray with " + std::string( typeName() ) );
        values.clear();
        typename T::value_type val;
        for ( size_t ii=0; ii<getChildrenCount(); ii++)
        {
            if( getChild(ii)->get_to( val ) )
                values.emplace_back(val);
        }
        return true;
    }

    /// Get map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class Map,
              class = typename std::enable_if<is_mappish<Map>{}, bool>::type >
    bool get_to_map( Map& values  ) const
    {
        JARANGO_THROW_IF( !isObject(), "JsonBase", 12, "cannot use getMapKey with " + std::string( typeName() ) );
        values.clear();
        using Tkey = std::remove_const_t<typename Map::value_type::first_type>; //may need to remove const
        using Tval = std::remove_const_t<typename Map::value_type::second_type>;

        Tval val;
        Tkey key;
        for ( size_t ii=0; ii<getChildrenCount(); ii++)
        {
            if( !string2v( getChild(ii)->getKey(), key ) )
                continue;
            if( getChild(ii)->get_to( val ) )
                 values.emplace( key, val);
        }
        return true;
    }

    // Set methods  --------------------------

    bool set_from( const std::string& value  )
    {
        update_node(  String, v2string(value) );
        return true;
    }

    /// Set Value to current Node
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        auto decodedType = typeTraits( value );
        if( decodedType>= Null && decodedType<=String )
        {
            update_node(  decodedType, v2string(value) );
            return true;
        }
        return false;
    }

    /// Set Array to current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        set_list_from( value );
        return true;
    }

    /// Set Map to current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        set_map_from( value );
        return true;
    }

    /// Set  vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    void set_list_from( const T& values  )
    {
        int ii{0};
        update_node(  Array, "" );
        for( const auto& el: values )
        {
            append_node( std::to_string(ii++), UNDEFINED, "" ).set_from(el);
        }
    }

    /// Set map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    void set_map_from( const T& values  )
    {
        update_node(  Object, "" );
        for( const auto& el: values )
        {
            append_node( el.first, UNDEFINED, "" ).set_from(el.second);
        }
    }

    // Update methods  --------------------------

    /// Clear field and set value to default (null).
    virtual bool clear()
    {
        update_node(  Null, "null" );
        return true;
    }

    /// Remove current field from json.
    virtual bool remove() = 0;

    /// Resize top level array
    //virtual bool resizeArray( const std::vector<std::size_t>& sizes, const std::string& defval  = "" ) = 0;

private:

    virtual void update_node(  Type atype, const std::string& avalue ) =0;
    virtual JsonBase& append_node( const std::string& akey, Type atype, const std::string& avalue ) =0;

public:

    /// Decode object type from value type
    template < class T>
    static Type typeTraits( const T& )
    {
        Type decodedType  = UNDEFINED;

        if( std::is_integral<T>::value )
        {
            decodedType = Int;
        }
        else if( std::is_floating_point<T>::value )
        {
            decodedType = Double;
        }
        else if( is_mappish<T>::value )
        {
            decodedType = Object;
        }
        else  if( is_container<T>::value )
        {
            decodedType = Array;
        }
        return   decodedType;
    }

    static Type typeTraits( const char * )
    {
        return String;
    }

    /// @brief return the type as string
    ///   Returns the type name as string to be used in error messages - usually to
    ///   indicate that a function was called on a wrong JSON type.
    static const char* typeName(Type type);


    friend class JsonBuilderBase;
    friend class JsonObjectBuilder;
    friend class JsonArrayBuilder;
};

std::ostream &operator<<(std::ostream& os, const JsonBase& obj);


} // namespace jsonio14
