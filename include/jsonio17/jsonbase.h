#pragma once

#include <iostream>
#include <vector>
#include <set>
#include "jsonio17/exceptions.h"
#include "jsonio17/type_test.h"
#include "jsonio17/jsondetail.h"

namespace jsonio17 {

using list_names_t =  std::vector<std::string>;
using set_names_t =  std::set<std::string>;
const std::string field_path_delimiters =  "./[]\"";

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
    virtual bool isNumber() const
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

    /// @brief Dump object to JSON string.
    std::string dump( bool dense = false ) const;

    /// Serialize object as a JSON formatted stream.
    void dump(std::ostream &os, bool dense = false) const;

    /// Deserialize a JSON document to a json object.
    virtual void loads( const std::string& jsonstr );

    /// Returns a std::string with the json contained in this object.
    virtual std::string toString(bool dense = false) const;
    /// Returns the object converted to a double value.
    virtual double toDouble() const;
    /// Returns the object converted to a long value.
    virtual long   toInt() const;
    /// Returns the object converted to a boolean value.
    virtual bool   toBool() const;

    /// Return field key
    virtual const std::string& getKey() const = 0;

    /// Get object type.
    virtual Type type() const = 0;

    /// @brief return the type as string
    ///   Returns the type name as string to be used in error messages - usually to
    ///   indicate that a function was called on a wrong JSON type.
    const char* typeName() const
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

    /// Get sizes of complex array ( 2D, 3D ... ).
    /// Important: get only sizes of first children.
    virtual std::vector<size_t> array_sizes() const = 0;

    virtual const JsonBase& child( size_t ii ) const
    {
        auto cild_ptr = getChild(ii);
        if( cild_ptr )
            return *cild_ptr;
        JSONIO_THROW( "JsonBase", 19, "cannot use child with " + std::to_string( ii ) );
    }

    // Get values  --------------------------

    /// Get internal data to json string
    bool get_to( std::string& value  ) const
    {
        value = toString(true);
        return true;
    }

    /// Explicit type conversion between the JSON value and a compatible primitive value.
    /// The value is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        auto decodedType = typeTraits( value );
        if( decodedType> Null && decodedType<=String )
        {
            return string2v( getFieldValue(), value );
        }
        return false;
    }

    /// Explicit type conversion between the JSON value and a compatible array-like value.
    /// The value is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class T,
              std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        return get_to_list( value );
    }

    /// Explicit type conversion between the JSON value and a compatible  map-like value.
    /// The value is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class T,
              std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        return get_to_map( value );
    }

    /// Explicit type conversion between the JSON value and a compatible array-like value.
    /// The value (std::list, std::vector ) is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    /// Container must have emplace_back function.
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    bool get_to_list( T& values  ) const
    {
        values.clear();
        if( isNull() )
            return true;
        //JSONIO_THROW_IF( !isStructured(), "JsonBase", 11, "cannot use getArray with " + std::string( typeName() ) );
        if( !isStructured() )
            return false;
        typename T::value_type val;
        for ( size_t ii=0; ii<getChildrenCount(); ii++)
        {
            if( getChild(ii)->get_to( val ) )
                values.emplace_back(val);
        }
        return true;
    }

    /// Explicit type conversion between the JSON value and a compatible  map-like value.
    /// The value (std::map, std::unordered_map ) is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class Map,
              class = typename std::enable_if<is_mappish<Map>{}, bool>::type >
    bool get_to_map( Map& values  ) const
    {
        values.clear();
        if( isNull() )
            return true;
        //JSONIO_THROW_IF( !isStructured(), "JsonBase", 12, "cannot use getMapKey with " + std::string( typeName() ) );
        if( !isStructured() )
            return false;
        using Tkey = std::remove_const_t<typename Map::value_type::first_type>;
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

    bool set_null()
    {
        update_node(  Null, "null" );
        return true;
    }

    bool set_from( const std::string& value  )
    {
        update_node(  String, /*v2string*/(value) );
        return true;
    }

    /// Set Value to current Node
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        auto decodedType = typeTraits( value );
        if( decodedType> Null && decodedType<=String )
        {
            update_node(  decodedType, v2string(value) );
            return true;
        }
        if( decodedType== Null)
        {
            set_null();
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
            auto obj = append_node( std::to_string(ii++), Null, "" );
            if( obj )
                obj->set_from(el);
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
            auto obj = append_node( el.first, Null, "" );
            if( obj )
                obj->set_from(el.second);
        }
    }

    // Update methods  --------------------------

    /// Clear field and set value to default (null).
    virtual bool clear() = 0;

    /// Remove current field from json.
    virtual bool remove() = 0;

    /// Resize array ( 1D, 2D, 3D ... ).
    /// Set up defval values if the JSON type of elements is primitive
    virtual void array_resize_xD( const std::vector<size_t> &sizes, const std::string& defval )
    {
        JSONIO_THROW_IF( !isArray(), "JsonBase", 11, "cannot resize not array data " + std::string( typeName() ) );
        resize_array_level( 0, sizes, defval  );
    }

    /// Resize 1D array.
    /// Set up defval values if the JSON type of elements is primitive.
    virtual void array_resize( std::size_t size, const std::string &defval ) = 0;


    // Field path  methods --------------------------

    /// Get field by fieldpath ("name1.name2.name3")
    virtual const JsonBase *field(  const std::string& fieldpath ) const;

    virtual JsonBase *field( std::queue<std::string> names ) const = 0;

    /// Return a string representation of the jsonpath to top field.
    std::string get_path() const;

    /// Check if a JSON object contains a certain jsonpath.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    virtual bool path_if_exists( const std::string& jsonpath ) const
    {
        return ( field( jsonpath ) != nullptr ) ;
    }

    /// Explicit type conversion between the JSON path value and a compatible primitive value.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// The value is filled into the input parameter.
    /// @return true if JSON value exist and can be converted to value type.
    template <typename T>
    bool get_value_via_path( const std::string& jsonpath, T& val, const T& defval   ) const
    {
        auto pobj = field( jsonpath );
        if( pobj && pobj->get_to(val) )
            return true;

        val = defval;
        return false;
    }

    /// Get key field from the JSON path value.
    bool get_key_via_path( const std::string& jsonpath, std::string& key, const std::string& defkey = "---"  ) const
    {
        auto pobj = field( jsonpath );
        if( pobj )
        {
            key =  pobj->getFieldValue();
            return true;
        }

        key = defkey;
        return false;
    }

    /// Use jsonpath to modify any value in a JSON object.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// @return true if jsonpath present in a JSON object.
    template <typename T>
    bool set_value_via_path( const std::string& jsonpath, const T& val  )
    {
        auto pobj = field_add( jsonpath );
        if( pobj )
            return pobj->set_from(val);

        return false;
    }

    /// Return a reference to object[jsonpath] if an object can be create, exception otherwise.
    virtual JsonBase &add_object_via_path(const std::string &jsonpath) = 0;

    /// Set _id to object
    virtual void set_oid( const std::string& oid  );

protected:

    // Get methods ( using in Qt GUI ) --------------------------

    virtual size_t getNdx() const = 0;

    virtual const std::string& getFieldValue() const = 0;

    virtual std::size_t getChildrenCount() const = 0;

    virtual const JsonBase* getChild( std::size_t ndx ) const = 0;

    virtual const JsonBase* getParent() const = 0;

    virtual list_names_t getUsedKeys() const = 0;

    /// Get object name.
    virtual std::string getHelpName() const;

    /// Get object description.
    virtual std::string getDescription() const
    {
        return getHelpName();
    }

private:

    virtual void update_node(  Type atype, const std::string& avalue ) =0;
    virtual JsonBase *append_node( const std::string& akey, Type atype, const std::string& avalue ) =0;
    void dump2stream(std::ostream &os, int depth, bool dense) const;
    // Get field by fieldpath ("name1.name2.name3")
    //JsonBase *field(  const std::string& fieldpath ) const;
    /// Get field by fieldpath ("name1.name2.name3")
    JsonBase *field_add(  const std::string& fieldpath );
    virtual JsonBase *field_add(std::queue<std::string> names) = 0;

    virtual void resize_array_level(size_t level, const std::vector<size_t> &sizes, const std::string &defval)=0;

public:

    /// Decode object type from value type
    template < class T>
    static Type typeTraits( const T& )
    {
        Type decodedType  = Null;

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

    static Type typeTraits( const char* )
    {
        return String;
    }

    /// @brief return the type as string
    ///   Returns the type name as string to be used in error messages - usually to
    ///   indicate that a function was called on a wrong JSON type.
    static const char* typeName(Type type);

    /// Return legal value for type.
    static std::string checked_value( JsonBase::Type type, const std::string &newvalue );

    friend class JsonBuilderBase;
    friend class JsonObjectBuilder;
    friend class JsonArrayBuilder;
};

std::ostream &operator<<(std::ostream& os, const JsonBase& obj);
template <> JsonBase::Type JsonBase::typeTraits( const bool& );
template <> JsonBase::Type JsonBase::typeTraits( const char& );
template <> JsonBase::Type JsonBase::typeTraits( const std::string& );

} // namespace jsonio17
