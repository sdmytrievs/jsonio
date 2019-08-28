#pragma once

#include "exceptions.h"
#include "jsonbase.h"

namespace jsonio14 {


/// @brief a class to store JSON object
class JsonFree final : public JsonBase
{

public:

   /// Create object JSON value
   static  JsonFree object( const std::string& key = "top" )
   {
        return JsonFree( Object, key, "" );
   }

   /// Create array JSON value
   static  JsonFree array( const std::string& key = "top" )
   {
       return JsonFree( Array, key, "" );
   }

    /// Copy constructor
    JsonFree(const JsonFree &obj );
    /// Move constructor
    JsonFree( JsonFree &&obj ) = default;
    //JsonFree( JsonFree &&obj ) noexcept;
    /// Destructor
    ~JsonFree() override {}

    /// Copy assignment
    JsonFree &operator =( const JsonFree &other)
    {
        if ( this != &other)
        {
            copy(other);
        }
        return *this;
    }

    /// Move assignment
    JsonFree &operator =( JsonFree &&other)
    {
        if ( this != &other)
        {
            move(std::move(other));
        }
        return *this;
    }

    /// Other type assignment
    template <class T>
    JsonFree &operator =( const T& value )
    {
       set_from( value  );
       return *this;
    }

    /// Return a const reference to arr[i] if this is an array, exception otherwise.
    const JsonFree &operator[](size_t idx) const
    {
        //std::cout << "const JsonFree &operator[](size_t idx)" << std::endl;
        if( isArray() || isObject()  )
        {
            return get_child( idx );
        }
        JARANGO_THROW( "JsonFree", 21, "cannot use operator[] with a numeric argument with " + std::string( typeName() ) );
    }

    /// Return a reference to arr[i] if this is an array, exception otherwise.
    JsonFree &operator[](size_t idx)
    {
        //std::cout << "JsonFree &operator[](size_t idx)" << std::endl;
        if( isArray() || isObject()  )
        {
            return get_child( idx );
        }
        JARANGO_THROW( "JsonFree", 22, "cannot use operator[] with a numeric argument with " + std::string( typeName() ) );
    }


    /// Return a const reference to object[key] if this is an object, exception otherwise.
    const JsonFree &operator[](const std::string &key) const
    {
        //std::cout << "const JsonFree &operator[](const std::string &key)" << std::endl;
        if( isObject()  )
        {
            return get_child( key );
        }
        JARANGO_THROW( "JsonFree", 23, "cannot use operator[] with a string argument with " + std::string( typeName() ) );
    }

    /// Return a reference to object[key] if this is an object, exception otherwise.
    JsonFree &operator[](const std::string &key)
    {
        //std::cout << "JsonFree &operator[](const std::string &key)" << std::endl;
        if( isNull() )
           update_node( JsonBase::Object, "" );
        if( isArray() || isObject()  )
        {
            return get_child( key );
        }
        JARANGO_THROW( "JsonFree", 24, "cannot use operator[] with a string argument with " + std::string( typeName() ) );
    }

    // Test methods  --------------------------------------------

    /// Test top object
    bool isTop() const override
    { return parent_object == nullptr; }


    // Get methods  --------------------------

    Type type() const override
    {   return  field_type;  }


    // Get methods ( using in Qt GUI model ) --------------------------

    const std::string& getKey() const override
    {   return  field_key;   }

    size_t getNdx() const override
    {   return ndx_in_parent;  }

    const std::string& getFieldValue() const override
    {   return  field_value;  }

    std::size_t getChildrenCount() const override
    {   return children.size();  }

    const JsonBase* getChild( std::size_t ndx ) const override
    {
        if( ndx < getChildrenCount() )
        {
            return  &children[ndx];
        }
        return nullptr;
    }

    const JsonBase* getParent() const override
    {  return parent_object;  }

    std::vector<std::string> getUsedKeys() const override;


    // Update methods  --------------------------


    // Remove current field
    bool remove() override;


private:

    /// Object type
    Type field_type;
    /// Object key  ( number for arrays )
    std::string field_key;
    /// Object value ( empty for arrays and objects )
    std::string field_value;

    /// Index into parent object ( using in GUI )
    size_t ndx_in_parent;
    /// Parent object
    JsonFree *parent_object;
    /// Children objects for Object or Array
    std::vector<JsonFree> children;

    /// Object constructor
    JsonFree( JsonBase::Type atype, const std::string &akey, const std::string& avalue, JsonFree *aparent = nullptr );

    void update_node(  JsonBase::Type atype, const std::string& avalue ) override;
    JsonFree &append_node( const std::string& akey, JsonBase::Type atype, const std::string& avalue ) override;
    /// Get field by fieldpath ("name1.name2.name3")
    JsonFree *field(  const std::string& fieldpath ) const override;
    /// Get field by fieldpath
    JsonFree *field( std::queue<std::string> names ) const override;

    /// Deep copy children
    void copy(const JsonFree &obj);
    /// Move children
    void move( JsonFree &&obj);

    const JsonFree &get_child(std::size_t idx) const;
    JsonFree &get_child( std::size_t idx );
    const JsonFree &get_child(const std::string& key) const;
    JsonFree &get_child(const std::string& key);
    JsonFree &get_parent() const;

    bool remove_child(JsonFree *child);
    bool remove_child(std::size_t idx);
    bool remove_child(const std::string &key);
};

// Add iterator?
// add resize array 2D, 3D ....
/// Get field by fieldpath ("name1.name2.name3")
//virtual JsonDom *field(  const std::string& fieldpath ) const = 0;
/// Get field by fieldpath
//virtual JsonDom *field( std::queue<std::string> names ) const = 0;
/// Get Value bhy path  from Node
/// If field is not number, the false will be returned.
//template <class T>
//bool findValue( const std::string& keypath, T& value  ) const
// Update functions  -----------------------------------
/// Resize top level Array
//virtual void resizeArray( const std::vector<std::size_t>& sizes, const std::string& defval  = "" ) = 0;
/// Set value to children Node
//template <class T>
//bool setFieldValue( const std::string& keypath, const T& value  )
//virtual bool updateTypeTop( int newtype ) = 0;
//
/// Set _id to Node
//virtual void setOid_( const std::string& _oid  );
/// Set up string json value to array or struct field
//void setComplexValue(const std::string& newval );

} // namespace jsonio14
