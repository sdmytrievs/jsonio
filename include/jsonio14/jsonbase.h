#pragma once
#include <vector>
#include "jsondetail.h"
#include "exceptions.h"

namespace jsonio14 {


/// @brief an interface to store JSON object
/// @class JsonBase represents an abstract item in a tree view.
/// Item data defines one json object
class JsonBase
{
protected:

   explicit JsonBase() {}
   /// Destructor
   virtual ~JsonBase(){}

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

    /// Test top object
    virtual bool isTop() const = 0;

    /// Test object as Struct
    virtual bool isObject() const
    { return getType() == Type::Object; }

    /// Test object as Array
    virtual bool isArray() const
    { return  getType() == Type::Array; }

    /// Test object as numeric value
    virtual bool isNumber( ) const
    { return  getType() == Type::Int or getType() == Type::Double; }

    /// Test object as Bool
    virtual bool isBool() const
    { return getType() == Type::Bool; }

    /// Test object as Null
    virtual bool isNull() const
    { return getType() == Type::Null; }

    /// Test object as String
    virtual bool isString() const
    { return getType() == Type::String; }

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
    virtual Type getType() const = 0;

    /// Get Value from object
    /// If object is not numeric value, the false will be returned.
    template <class T>
    bool getValueOld( T& value  ) const
    {
       switch( getType() )
       {
        case Type::Double: value = toDouble(); break;
        case Type::Int:    value = toInt();  break;
        case Type::Bool:   value = toBool();  break;
        default:     return false;
       }
       return true;
    }

    bool getValueOld( std::string& value  ) const
    {
       value = toString( true );
       return true;
    }



    // Get methods ( using in Qt GUI ) --------------------------

    virtual const std::string& getKey() const = 0;

    virtual size_t getNdx() const = 0;

    virtual const std::string& getFieldValue() const = 0;

    virtual std::size_t getChildrenCount() const = 0;

    virtual const JsonBase* getChild( std::size_t ndx ) const = 0;

    virtual const JsonBase* getParent() const = 0;

    virtual std::vector<std::string> getUsedKeys() const = 0;

    /// Get object name
    virtual std::string getHelpName() const;

    /// Get object description
    virtual std::string getDescription() const
    {
        return getHelpName();
    }

    /// Get field path to top object
    std::string getFieldPath() const;

    // Set methods  --------------------------

    /// Set Value to current Node
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool setValue( const T& value  )
    {
        auto decodedType = typeTraits( value );
        if( decodedType>= Null && decodedType<=String )
        {
            updateNodeData(  decodedType, v2string(value) );
            return true;
        }
        return false;
    }

    /// Set Array to current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool setValue( const T& value  )
    {
        setArray( value );
        return true;
    }

    /// Set Map to current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
    bool setValue( const T& value  )
    {
        setMapKey( value );
        return true;
    }

    /// Set  vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    void setArray( const T& values  )
    {
        int ii{0};
        updateNodeData(  Array, "" );
        for( const auto& el: values )
        {
            appendNode( std::to_string(ii++), UNDEFINED, "" ).setValue(el);
        }
    }

    /// Set map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    void setMapKey( const T& values  )
    {
        updateNodeData(  Object, "" );
        for( const auto& el: values )
        {
            appendNode( el->first, UNDEFINED, "" ).setValue(el->second);
        }
    }


private:

    virtual void updateNodeData(  Type atype, const std::string& avalue ) =0;
    virtual JsonBase& appendNode( const std::string& akey, Type atype, const std::string& avalue ) =0;

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

    const char* typeName() const
    {
        switch( getType() )
        {
        case Null:
            return "null";
        case Object:
            return "object";
        case Array:
            return "array";
        case String:
            return "string";
        case Bool:
            return "boolean";
        case UNDEFINED:
            return "undefined";
        default:
            return "number";
        }
    }


};


} // namespace jsonio14
