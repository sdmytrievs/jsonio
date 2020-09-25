#pragma once

#include <memory>
#include <algorithm>

#include "jsonio17/exceptions.h"
#include "jsonio17/jsonbase.h"
#include "jsonio17/schema.h"

namespace jsonio17 {


/// @brief a class to store schema based JSON object
class JsonSchema final : public JsonBase
{

public:

    /// An iterator for a JsonSchema container
    using iterator = std::vector<std::shared_ptr<JsonSchema>>::iterator;
    /// A const iterator for a JsonSchema container
    using const_iterator = std::vector<std::shared_ptr<JsonSchema>>::const_iterator;

    /// Create object JSON value
    static  JsonSchema object( const std::string& aschema_name );

    // Create array JSON value
    //static  JsonSchema array( const std::string& aschema_name, const std::string& key = "top" )
    //{
    //    return JsonSchema( Array, key, "", nullptr );
    //}

    /// Copy constructor
    JsonSchema(const JsonSchema &obj );
    /// Move constructor
    JsonSchema( JsonSchema &&obj ) noexcept;
    /// Destructor
    ~JsonSchema() override {}

    /// Copy assignment
    JsonSchema &operator =( const JsonSchema &other)
    {
        if ( this != &other)
        {
            // exception if other schema
            test_assign_object(other);
            copy(other);
        }
        return *this;
    }

    /// Move assignment
    JsonSchema &operator =( JsonSchema &&other)
    {
        if ( this != &other)
        {
            // exception if other schema
            test_assign_object(other);
            move(std::move(other));
        }
        return *this;
    }

    /// Other type assignment
    template <class T>
    JsonSchema &operator =( const T& value )
    {
        this->set_from( value  );
        return *this;
    }

    /// Return a const reference to arr[i] if this is an array, exception otherwise.
    const JsonSchema &operator[](size_t idx) const
    {
        //std::cout << "const JsonSchema &operator[](size_t idx)" << std::endl;
        if( isArray() )
        {
            return get_child( idx );
        }
        JSONIO_THROW( "JsonSchema", 21, "cannot use operator[] with a numeric argument with " + std::string( typeName() ) );
    }

    /// Return a reference to arr[i] if this is an array, exception otherwise.
    JsonSchema &operator[](size_t idx)
    {
        //std::cout << "JsonSchema &operator[](size_t idx)" << std::endl;
        if( isArray() )
        {
            return get_child( idx );
        }
        JSONIO_THROW( "JsonSchema", 22, "cannot use operator[] with a numeric argument with " + std::string( typeName() ) );
    }


    /// Return a const reference to object[key] if this is an object, exception otherwise.
    const JsonSchema &operator[](const std::string &key) const
    {
        //std::cout << "const JsonSchema &operator[](const std::string &key)" << std::endl;
        if( isObject()  )
        {
            return get_child( key );
        }
        JSONIO_THROW( "JsonSchema", 23, "cannot use operator[] with a string argument with " + std::string( typeName() ) );
    }

    /// Return a reference to object[key] if this is an object, exception otherwise.
    JsonSchema &operator[](const std::string &key)
    {
        //std::cout << "JsonSchema &operator[](const std::string &key)" << std::endl;
        if(  isObject()  )
        {
            return get_child( key );
        }
        JSONIO_THROW( "JsonSchema", 24, "cannot use operator[] with a string argument with " + std::string( typeName() ) );
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
    const_iterator cend() const
    {
        return children.cend();
    }

    /// @brief returns a const iterator to the first element
    const_iterator cbegin() const
    {
        return children.cbegin();
    }

    // Test methods  --------------------------------------------

    /// This function returns true if and only if the JSON object is top.
    bool isTop() const override
    { return parent_object == nullptr; }

    /// This function returns true if and only if the JSON type is defined structure Object.
    bool isStruct() const
    { return( isTop() or  fieldType() == FieldDef::T_STRUCT ); }

    /// This function returns true if and only if the JSON type is free structure Object.
    bool isMap() const
    { return( !isTop() and fieldType() == FieldDef::T_MAP );  }

    bool isUnion( ) const
    {  return ( !isTop() and parent_object->isStruct() && parent_object->struct_descrip->isUnion() );  }

    // Get methods  --------------------------

    /// Get fields key type for Object
    FieldDef::FieldType fieldKeyType() const
    {
        if( !isTop() )
            return  parent_object->field_descrip->type(parent_object->level_type+1);
        return FieldDef::T_STRING;
    }

    /// Get field type for Object
    FieldDef::FieldType fieldType() const
    {
        return field_descrip->type(level_type);
    }

    /// Get type of elements (back) for Object
    FieldDef::FieldType elemType() const
    {
        return field_descrip->elemType();
    }

    const std::string& getKey() const override
    {   return  field_key;   }

    /// Get json type from Object
    Type type() const override
    {
        return  fieldtype2basetype( fieldType() );
    }

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

    /// Get field by fieldpath ("name1.name2.name3")
    const JsonSchema *field(  const std::string& fieldpath ) const override;

    /// Return a reference to object[jsonpath] if an object can be create, exception otherwise.
    JsonSchema &add_object_via_path(const std::string &jsonpath) override;

    /// Return a reference to object[jsonpath] if an array can be create, exception otherwise.
    JsonSchema &add_array_via_path(const std::string &jsonpath) override;

protected:

    // Get methods ( using in Qt GUI model ) --------------------------

    size_t getNdx() const override
    {   return ndx_in_parent;  }

    const std::string& getFieldValue() const override
    {   return  field_value;  }

    std::size_t getChildrenCount() const override
    {   return children.size();  }

    JsonBase* getChild( std::size_t ndx ) const override
    {
        if( ndx < getChildrenCount() )
        {
            return  children[ndx].get();
        }
        return nullptr;
    }
    JsonBase* getParent() const override
    {
        return parent_object;
    }

    /// Generate list of non existing fields
    list_names_t getNoUsedKeys() const;
    /// Generate list of existing fields
    list_names_t getUsedKeys() const override;

    /// Get object name.
    virtual std::string getHelpName() const override;

    /// Get object description.
    virtual std::string getDescription() const override;

    /// Get Description from Node
    std::string getFullDescription() const;

    /// Get min Value to Node
    double minValue() const
    {
        return field_descrip->minValue();
    }

    /// Get max Value to Node
    double maxValue() const
    {
        return field_descrip->maxValue();
    }

private:

    /// Link to parent structure descriptions
    const  StructDef*  struct_descrip;
    /// Link to field description in structure
    const  FieldDef* const  field_descrip;
    /// Index into fldDef->fTypeId array
    std::size_t level_type;

    /// Object key  ( number for arrays )
    std::string field_key;
    /// Object value ( empty for arrays and objects )
    std::string field_value;

    /// Index into parent object ( using in GUI )
    size_t ndx_in_parent;
    /// Parent object
    JsonSchema *parent_object;
    /// Children objects for Object or Array
    std::vector<std::shared_ptr<JsonSchema>> children;

    /// Top level constructor
    JsonSchema( const StructDef *aStrDef );
    /// Object constructor for fields
    JsonSchema( const FieldDef *afldDef, JsonSchema *aparent );
    /// Object constructor
    JsonSchema( JsonBase::Type atype, const std::string &akey, const std::string& avalue, JsonSchema *aparent  );

    void update_node(  JsonBase::Type atype, const std::string& avalue ) override;
    JsonSchema *append_node( const std::string& akey, JsonBase::Type atype, const std::string& avalue ) override;

    /// Get field by fieldpath
    JsonSchema *field( std::queue<std::string> names ) const override;
    /// Get field by idspath
    JsonSchema *field( std::queue<int> ids ) const;
    /// Add field by fieldpath
    JsonSchema *field_add(std::queue<std::string> names) override;

    /// Deep copy children
    void copy(const JsonSchema &obj);
    /// Move children
    void move( JsonSchema &&obj);

    auto find_key( const std::string &key ) const
    {
        return std::find_if( children.begin(), children.end(),
                             [=]( const auto& value ) { return value->getKey() == key; });
    }

    const JsonSchema &get_child(std::size_t idx) const;
    JsonSchema &get_child( std::size_t idx );
    const JsonSchema &get_child(const std::string& key) const;
    JsonSchema &get_child(const std::string& key);
    JsonSchema &get_parent() const;

    bool remove_child(JsonSchema *child);
    bool remove_child(std::size_t idx);
    bool remove_child(const std::string &key);

    void resize_array_level(size_t level, const std::vector<size_t> &sizes, const std::string &defval) override;

    //---------------------------------------------------------------------

    /// Test if copy or move assignment object possible
    bool test_assign_object( const JsonSchema &other, bool use_exception = true )
    {
        if ( this != &other)
        {
            if( !( struct_descrip == other.struct_descrip  and
                   ( FieldDef::topfield() == other.field_descrip or
                     ( field_descrip == other.field_descrip and level_type == other.level_type)))  )
            {
                if( use_exception )
                    JSONIO_THROW( "JsonSchema", 10, "copy or move assignment unpossible"  );
                else
                    return false;
            }
        }
        return true;
    }

    /// Test if assignment of value is possible
    bool test_assign_value( JsonBase::Type atype, bool use_exception = true  )
    {
        auto obj_type = type();
        if( !( obj_type == atype or
               atype == Null or
               ( ( obj_type == Type::Int or obj_type == Type::Double ) and
                 ( atype == Type::Int or atype == Type::Double ) ) ) )
        {
            if( use_exception )
                JSONIO_THROW( "JsonSchema", 11, "assignment of value of type is unpossible " + std::string( typeName() ) );
            else
                return false;
        }
        return true;
    }

    /// Type conversion from schema to json types
    JsonBase::Type fieldtype2basetype(FieldDef::FieldType field_type) const;

    /// Set up default children list ( empty if no structure )
    void set_children();
    /// Generate default children list for structure
    void struct2model( const StructDef * const strDef );
    /// Change current value from json string value
    void set_value(const std::string &value);

    /// Set default value
    void set_default_value()
    {
        if( level_type==0 and !field_descrip->defaultValue().empty() )
            loads( field_descrip->defaultValue() );
        if( field_descrip->className() == "TimeStamp" )
            set_current_time();
    }

    /// Set up current time field
    void set_current_time();
};


} // namespace jsonio17
