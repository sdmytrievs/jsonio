#include <algorithm>
#include "jsonfree.h"

namespace jsonio14 {

JsonFree::JsonFree( JsonBase::Type atype, const std::string &akey, const std::string &avalue, JsonFree *aparent ):
    field_type(atype), field_key(akey), field_value(avalue), ndx_in_parent(0), parent_object(aparent), children()
{
    if(parent_object)
    {
        ndx_in_parent = parent_object->children.size();
    }
}


JsonFree::JsonFree( const JsonFree &obj ):
    field_type(obj.field_type), field_key(obj.field_key), field_value(obj.field_value),
    ndx_in_parent(0), parent_object(nullptr), children()
{
    copy(obj);
}

/*JsonFree::JsonFree( JsonFree &&obj )noexcept:
    field_type(obj.field_type), field_key( obj.field_key ), field_value( "" ),
    ndx_in_parent(obj.ndx_in_parent), parent_object(obj.parent_object)
{
    move(std::move(obj));
}*/

std::vector<std::string> JsonFree::getUsedKeys() const
{
    std::vector<std::string> usekeys;
    for( const auto& el: children )
    {
        usekeys.push_back( el.getKey() );
    }
    return usekeys;
}

bool JsonFree::remove()
{
    if( parent_object == nullptr )
        return false;
    return parent_object->remove_child( this );
}


void JsonFree::update_node(JsonBase::Type atype, const std::string &avalue)
{
   children.clear();
   field_type = atype;
   field_value = avalue;
}

JsonFree& JsonFree::append_node(const std::string &akey, JsonBase::Type atype, const std::string &avalue )
{
  children.emplace_back( JsonFree{atype, akey, avalue, this} );
  return children.back();
}


JsonFree *JsonFree::field(std::queue<std::string> names) const
{
    if( names.empty() )
        return const_cast<JsonFree *>(this);

    auto fname = names.front();
    names.pop();

    auto element = std::find_if( children.begin(), children.end(),
                                 [=]( auto value ) { return value.getKey() == fname; });
    if( element == children.end() )
    {
        return nullptr;
    }
    return element->field(names);
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
        return append_node( key, JsonBase::Null, "" );
        //return children.back();
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

bool JsonFree::remove_child( std::size_t idx )
{
  return remove_child( &get_child( idx ) );
}

bool JsonFree::remove_child( const std::string &key )
{
   return remove_child( &get_child( key ) );
}

bool JsonFree::remove_child( JsonFree* child )
{
    int thisndx = -1;
    for(std::size_t ii=0; ii< children.size(); ii++ )
    {
        if( &children[ii] == child )
            thisndx = static_cast<int>(ii);
        if( thisndx >= 0 )
            children[ii].ndx_in_parent--;
    }
    if( thisndx >= 0 )
    {   children.erase(children.begin() + thisndx);
        return true;
    }
    return false;
}


} // namespace jsonio14
