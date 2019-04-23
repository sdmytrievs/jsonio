#include <algorithm>
#include "jsonfree.h"

namespace jsonio14 {

JsonFree::JsonFree( JsonBase::Type atype, const std::string &akey, const std::string &avalue, JsonFree *aparent ):
    field_type(atype), field_key(akey), field_value(avalue), ndx_in_parent(0), parent_object(aparent)
{
    if(parent_object)
    {
        ndx_in_parent = parent_object->children.size();
    }
}

void JsonFree::update_node(JsonBase::Type atype, const std::string &avalue)
{
   children.clear();
   field_type = atype;
   field_value = avalue;
}

JsonBase& JsonFree::append_node(const std::string &akey, JsonBase::Type atype, const std::string &avalue )
{
  children.emplace_back( JsonFree{atype, akey, avalue, this} );
  return children.back();
}

JsonFree::JsonFree( const JsonFree &obj ):
    field_type(obj.field_type), field_key(obj.field_key), field_value(obj.field_value),
    ndx_in_parent(0), parent_object(nullptr)
{
    copy(obj);
}

JsonFree::JsonFree( JsonFree &&obj ):
    field_type(obj.field_type), field_key( std::move(obj.field_key) ), field_value( "" ),
    ndx_in_parent(0), parent_object(nullptr)
{
    move(std::move(obj));
}

std::vector<std::string> JsonFree::getUsedKeys() const
{
    std::vector<std::string> usekeys;
    for( const auto& el: children )
    {
        usekeys.push_back( el.getKey() );
    }
    return usekeys;
}

// key and parent not changed
void JsonFree::copy(const JsonFree &obj)
{
    field_type =  obj.field_type;
    field_value = obj.field_value;

    children.clear();
    for( auto child: obj.children )
    {
        children.push_back( JsonFree(child) );
        children.back().parent_object = this;
        children.back().ndx_in_parent = children.size()-1;
    }
}

// key and parent not changed
void JsonFree::move(JsonFree &&obj)
{
    field_type =  obj.field_type;
    field_value = std::move(obj.field_value);
    children = std::move(obj.children);
}

const JsonFree& JsonFree::get_child(std::size_t idx) const
{
    JARANGO_THROW_IF( idx>=getChildrenCount(), "JsonFree", 25,  "array index " + std::to_string(idx) + " is out of range" );
    return children[idx];
}

JsonFree& JsonFree::get_child(std::size_t idx)
{
    JARANGO_THROW_IF( idx>=getChildrenCount(), "JsonFree", 25, "array index " + std::to_string(idx) + " is out of range" );
    return children[idx];
}

JsonFree& JsonFree::get_child(const std::string &key)
{
    auto element = std::find_if( children.begin(), children.end(),
                                 [=]( auto value ) { return value.getKey() == key; });
    if( element == children.end() )
    {
        append_node( key, JsonBase::Object, "" );
        return children.back();
    }
    return *element;
}


const JsonFree& JsonFree::get_child(const std::string &key) const
{
    auto element = std::find_if( children.begin(), children.end(),
                                 [=]( auto value ) { return value.getKey() == key; });
    if( element == children.end() )
    {
        JARANGO_THROW( "JsonFree", 26, "key '" + key + "' not found" );
    }
    return *element;
}


JsonFree &JsonFree::get_parent() const
{
    JARANGO_THROW_IF( !parent_object, "JsonFree", 27, "parent Object is undefined" );
    return *parent_object;
}



/* old
 *
 *

    /// Set Value to current Node
    template <class T>
    bool setValue( const T& value  )
    {
        auto decodedType = typeTraits( value );
        switch( decodedType )
        {
        case Null:
        case Bool:
        case Int:
        case Double:
        case String:  updateNodeData(  decodedType, v2string(value) );
            break;
        case Array:   //setArray( value );
            break;
        case Object: // setMapKey( value );
            break;
        default:
            return false;
        }
        return true;
    }

  */
} // namespace jsonio14
