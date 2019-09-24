#pragma once

#include "exceptions.h"
#include "jsonbase.h"

namespace jsonio14 {


/// @brief a class to store JSON object
class JsonFree final : public JsonBase
{

public:

    /// An iterator for a JsonFree container
    using iterator = std::vector<JsonFree>::iterator;
    /// A const iterator for a JsonFree container
    using const_iterator = std::vector<JsonFree>::const_iterator;

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
    JsonFree( JsonFree &&obj ) noexcept;
    //JsonFree( JsonFree &&obj ) = default; not worked
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

    /// @brief returns an iterator to one past the last element.
    iterator end()
    {
        return children.end();
    }

    /// @brief returns an iterator to the first element.
    iterator begin()
    {
        return children.begin();
    }

    /// @brief returns a const iterator to one past the last element.
    const_iterator cend()
    {
        return children.cend();
    }

    /// @brief returns a const iterator to the first element
    const_iterator cbegin() const
    {
        return children.cbegin();
    }

    // Test methods  --------------------------------------------

    /// Test top object
    bool isTop() const override
    { return parent_object == nullptr; }

    // Get methods  --------------------------

    Type type() const override
    {   return  field_type;  }

    /// Get sizes of complex array ( 2D, 3D ... ).
    /// Important: get only sizes of first children.
    std::vector<size_t> array_sizes() const override;


    // Update methods  --------------------------

    /// Clear field and set value to default (empty or 0).
    virtual bool clear() override;

    /// Remove current field.
    bool remove() override;

    /// Resize 1D array.
    /// Set up defval values if the JSON type of elements is primitive.
    void array_resize( std::size_t size, const std::string &defval ) override;

    /// Return a reference to object[jsonpath] if an object can be create, exception otherwise.
    virtual JsonFree &add_object_via_path(const std::string &jsonpath) override;

protected:

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
    /// Get field by fieldpath
    JsonFree *field( std::queue<std::string> names ) const override;
    /// Add field by fieldpath
    JsonFree *field_add(std::queue<std::string> names) override;

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

    void resize_array_level(size_t level, const std::vector<size_t> &sizes, const std::string &defval) override;

};

// Add iterator?
//virtual bool updateTypeTop( int newtype ) = 0;
//
/// Set _id to Node
//virtual void setOid_( const std::string& _oid  );
//
/// Set up string json value to array or struct field
//void setComplexValue(const std::string& newval );

} // namespace jsonio14
