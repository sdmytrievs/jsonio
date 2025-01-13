#include "jsonio/jsonfree.h"
#include "jsonio/jsonbuilder.h"

namespace jsonio {

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

JsonFree::JsonFree( JsonFree &&obj ) noexcept:
    field_type(obj.field_type), field_key( obj.field_key ), field_value( "" ),
    ndx_in_parent(obj.ndx_in_parent), parent_object(obj.parent_object), children()
{
    move(std::move(obj));
}



list_names_t JsonFree::getUsedKeys() const
{
    list_names_t usekeys;
    for( const auto& el: children )
    {
        usekeys.push_back( el->getKey() );
    }
    return usekeys;
}

JsonFree *JsonFree::getChild(const std::string &key) const
{
    auto element = find_key(key);
    if( element == children.end() )
        return nullptr;
    return element->get();
}


bool JsonFree::clear()
{
    children.clear();
    if( isBool() )
        field_value = "false";
    else if( isNumber() )
        field_value = "0";
    else
        field_value = "";

    return true;
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

JsonFree *JsonFree::append_node(const std::string &akey, JsonBase::Type atype, const std::string &avalue )
{
    auto shptr = new JsonFree(atype, akey, avalue, this);
    children.push_back( std::shared_ptr<JsonFree>(shptr) );
    return children.back().get();
}

const JsonFree *JsonFree::field(const std::string &fieldpath ) const
{
    auto names = split( fieldpath, field_path_delimiters );
    return field( names );
}

JsonFree *JsonFree::field( std::queue<std::string> names ) const
{
    if( names.empty() )
        return const_cast<JsonFree *>(this);

    auto fname = names.front();
    names.pop();

    auto element = find_key(fname);
    if( element == children.end() )
    {
        return nullptr;
    }
    return element->get()->field(names);
}

JsonFree *JsonFree::field_add_names(std::queue<std::string> names )
{
    if( names.empty() )
        return const_cast<JsonFree *>(this);

    auto fname = names.front();
    names.pop();

    auto element = find_key(fname);
    if( element == children.end() )
    {
        if( isObject() )
        {
            append_node( fname, JsonBase::Object, "" );
            return children.back()->field_add_names(names);
        }
        else if( isArray() && fname== std::to_string(children.size()) )
        {
            append_node( fname, JsonBase::Object, "" );
            return children.back()->field_add_names(names);
        }
        else
            return nullptr;
    }
    return element->get()->field_add_names(names);
}

JsonFree &JsonFree::add_object_via_path(const std::string &jsonpath)
{
    auto names = split(jsonpath, field_path_delimiters);
    auto pobj = field_add_names( names );
    if( pobj )
    {
        if( !pobj->isObject())
            pobj->update_node( JsonBase::Object, "" );
        return *pobj;
    }
    JSONIO_THROW( "JsonBase", 12, "cannot create object with jsonpath " + std::string( jsonpath ) );
}

JsonFree &JsonFree::add_array_via_path(const std::string &jsonpath)
{
    auto names = split(jsonpath, field_path_delimiters);
    auto pobj = field_add_names( names );
    if( pobj )
    {
        if( !pobj->isArray())
            pobj->update_node( JsonBase::Array, "" );
        return *pobj;
    }
    JSONIO_THROW( "JsonBase", 21, "cannot create array with jsonpath " + std::string( jsonpath ) );
}

void JsonFree::placehold_null()
{
    for( auto& child:  children ) {
        if(child->isNull()) {
            child->update_node(String, "---");
        }
        else {
           child->placehold_null();
        }

    }
}

// key and parent not changed
void JsonFree::copy(const JsonFree &obj)
{
    field_type =  obj.field_type;
    // field_key =  obj.field_key; // copy only data
    field_value = obj.field_value;

    children.clear();
    for( const auto& child: obj.children )
    {
        children.push_back( std::make_shared<JsonFree>(*child) );
        children.back()->parent_object = this;
        children.back()->ndx_in_parent = children.size()-1;
    }
}

// key and parent not changed
void JsonFree::move(JsonFree &&obj)
{
    field_type =  obj.field_type;
    // field_key =  std::move(obj.field_key);  // stl using
    field_value = std::move(obj.field_value);

    children = std::move(obj.children);
    obj.children.clear();
    for( const auto& child: children )
    {
        child->parent_object = this;
        //children.back()->parent_object = this;
    }
}

const JsonFree& JsonFree::get_child(std::size_t idx) const
{
    JSONIO_THROW_IF( idx>=getChildrenCount(), "JsonFree", 25,  "array index " + std::to_string(idx) + " is out of range" );
    return *children[idx];
}

JsonFree& JsonFree::get_child(std::size_t idx)
{
    JSONIO_THROW_IF( idx>getChildrenCount(), "JsonFree", 25, "array index " + std::to_string(idx) + " is out of range" );
    if( idx==getChildrenCount() ) // next element
    {
        append_node( std::to_string(idx), JsonBase::Null, "" );
        return *children.back();
    }
    return *children[idx];
}

JsonFree& JsonFree::get_child(const std::string &key)
{
    auto element = find_key(key);
    if( element == children.end() )
    {
        append_node( key, JsonBase::Null, "" );
        return *children.back();
    }
    return *element->get();
}

const JsonFree& JsonFree::get_child(const std::string &key) const
{
    auto element = find_key(key);
    if( element == children.end() )
    {
        JSONIO_THROW( "JsonFree", 26, "key '" + key + "' not found" );
    }
    return *element->get();
}


JsonFree &JsonFree::get_parent() const
{
    JSONIO_THROW_IF( !parent_object, "JsonFree", 27, "parent Object is undefined" );
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
        if( children[ii].get() == child )
            thisndx = static_cast<int>(ii);
        if( thisndx >= 0 )
        {
          children[ii]->ndx_in_parent--;
          if( isArray() )
              children[ii]->field_key = std::to_string(children[ii]->ndx_in_parent);
        }
    }
    if( thisndx >= 0 )
    {   children.erase(children.begin() + thisndx);
        return true;
    }
    return false;
}

std::vector<size_t> JsonFree::array_sizes() const
{
    std::vector<size_t> sizes;

    if( !isArray() )
        return sizes;

    if( !empty() )
    {
        sizes = children[0]->array_sizes();
    }

    sizes.insert( sizes.begin(), size() );
    return sizes;
}


void JsonFree::resize_array_level( size_t level, const std::vector<size_t>& sizes, const std::string& defval  )
{
    if( sizes.size() <= level )
        return;

    auto arsize = sizes[level];

    // Resize current level
    array_resize( arsize, defval  );

    // Resize next level
    for( auto& child:  children )
        child->resize_array_level( level+1, sizes, defval );
}

void JsonFree::array_resize( std::size_t  newsize, const std::string& defval  )
{
    JSONIO_THROW_IF( !isArray(), "JsonFree", 11, "cannot resize not array data " + std::string( typeName() ) );

    if( newsize == children.size() )     // the same size
        ;
    else if( newsize < children.size() ) // delete if smaler
        children.erase( children.begin()+newsize, children.end());
    else
    {
        if( children.size()>0 )
        {
            const auto defchild = children[0];
            auto chdefval = checked_value( defchild->type(), defval );

            for( auto ii=children.size(); ii<newsize; ii++)
            {
                children.emplace_back( std::make_shared<JsonFree>(*defchild) );
                children.back()->field_key = std::to_string(ii);
                children.back()->ndx_in_parent = ii;
                children.back()->parent_object = this;

                if( !defval.empty() && defchild->isPrimitive() )
                    children.back()->field_value = chdefval;
            }
        }
        else
        {
            JsonArrayBuilder jsBuilder(this);
            for( size_t ii=0; ii<newsize; ii++)
                jsBuilder.addScalar( defval );
        }
    }
}


} // namespace jsonio
