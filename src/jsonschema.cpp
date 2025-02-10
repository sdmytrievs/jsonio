#include <algorithm>
#include <ctime>
#include "jsonio/jsonschema.h"
#include "jsonio/jsonbuilder.h"
#include "jsonio/io_settings.h"


namespace jsonio {


JsonSchema JsonSchema::object( const std::string &schema_name )
{
    auto strDef  = ioSettings().Schema().getStruct( schema_name );
    JSONIO_THROW_IF( strDef == nullptr, "JsonSchema", 8, "undefined struct definition " + schema_name  );
    return JsonSchema( strDef );
}

JsonSchema::JsonSchema( const JsonSchema &obj ):
    struct_descrip(obj.struct_descrip),
    field_descrip(obj.field_descrip),
    level_type(obj.level_type),
    field_key(obj.field_key), field_value(obj.field_value),
    ndx_in_parent(0), parent_object(nullptr), children()
{
    copy(obj);
}

JsonSchema::JsonSchema( JsonSchema &&obj ) noexcept:
    struct_descrip(obj.struct_descrip),
    field_descrip(obj.field_descrip),
    level_type(obj.level_type),
    field_key( obj.field_key ), field_value( "" ),
    ndx_in_parent(obj.ndx_in_parent), parent_object(obj.parent_object), children()
{
    move(std::move(obj));
}

// Constructor for empty struct (up level )
JsonSchema::JsonSchema( const  StructDef* aStrDef ):
    struct_descrip( aStrDef ), field_descrip( FieldDef::topfield() ),
    level_type( 0 ),
    field_key( aStrDef->name() ), field_value( "" ),
    ndx_in_parent( 0 ),
    parent_object( nullptr ), children()
{
    set_children();
}

// Constructor for empty field
JsonSchema::JsonSchema( const  FieldDef* afldDef, JsonSchema* aparent ):
    struct_descrip( aparent->struct_descrip ), field_descrip( afldDef ),
    level_type( 0 ),
    field_key( afldDef->name() ), field_value( "" ),
    ndx_in_parent( aparent->children.size() ),
    parent_object( aparent ), children()
{
    //clear_setup();
    set_children();
    set_default_value();
}

JsonSchema::JsonSchema( JsonBase::Type , const std::string &akey, const std::string &avalue, JsonSchema *aparent ):
    struct_descrip( aparent->struct_descrip ), field_descrip( aparent->field_descrip ),
    level_type( 0 ),
    field_key( akey ), field_value( avalue ),
    ndx_in_parent(aparent->children.size()),
    parent_object(aparent), children()
{
    switch(parent_object->fieldType())
    {
    case FieldDef::T_MAP:
        level_type = parent_object->level_type+2;
        break;
    case FieldDef::T_SET:
    case FieldDef::T_LIST: // !! next level
        level_type = parent_object->level_type+1;
        break;
    default:
        break;
        //JSONIO_THROW( "JsonSchema", 9, "illegal parent type " + std::string( parent_object->typeName() )  );
    }
    set_children();
}

// key and parent not changed
void JsonSchema::copy(const JsonSchema &obj)
{
    // field_key =  obj.field_key; // must be the same
    field_value = obj.field_value;
    children.clear();
    for( const auto& child: obj.children )
    {
        children.push_back( std::make_shared<JsonSchema>(*child) );
        children.back()->parent_object = this;
        children.back()->ndx_in_parent = children.size()-1;
    }
}

// key and parent not changed
void JsonSchema::move(JsonSchema &&obj)
{
    field_value = std::move(obj.field_value);
    children = std::move(obj.children);
    obj.children.clear();
    for( const auto& child: children )
    {
        child->parent_object = this;
    }
}

void JsonSchema::update_node( JsonBase::Type atype, const std::string &avalue )
{
    // Test if type is possible otherwise exeption
    test_assign_value( atype );
    // set defaults for isStruct
    set_children();
    // set value from (json)string
    set_value(atype, avalue);
}

JsonSchema *JsonSchema::append_node(const std::string &akey, JsonBase::Type atype, const std::string &avalue )
{
    // if exist key => update node, for Free json too
    auto element = find_key(akey);
    if( element != children.end() )
    {
        if( element->get()->test_assign_value( atype, false ) ) // hide exception
            element->get()->update_node( atype, avalue );
        return element->get();
    }

    if( isStruct() )
    {
        // must be defined keys list
        auto  field_desc = struct_descrip->getField(akey);
        if( field_desc )
        {
            auto shptr = std::shared_ptr<JsonSchema>( new JsonSchema( field_desc, this ) );
            // add only type is possible
            if( shptr->test_assign_value( atype, false ) )
                shptr->update_node( atype, avalue );

            auto flds_before =struct_descrip->getFieldsBefore(akey);
            auto fld_insert_before = children.begin();
            while( fld_insert_before != children.end() &&
                   flds_before.find( fld_insert_before->get()->getKey() ) != flds_before.end() )
                fld_insert_before++;

            if( fld_insert_before != children.end() )
            {
              auto fields_after =  fld_insert_before;
              while( fields_after != children.end() )
              {
                 fields_after->get()->ndx_in_parent++;
                 fields_after++;
              }
            }

            // add by order
            shptr->ndx_in_parent = fld_insert_before-children.begin();
            return children.insert( fld_insert_before, shptr )->get();
        }
        return nullptr;
    }
    else if( isMap() || isArray() )
    {
        auto shptr = std::shared_ptr<JsonSchema>( new JsonSchema( atype, akey, avalue, this ) );
        // add only type is possible
        if( shptr->test_assign_value( atype, false ) )
        {
            children.push_back( shptr );
            return children.back().get();
        }
    }
    return nullptr;
}

const JsonSchema& JsonSchema::get_child(std::size_t idx) const
{
    JSONIO_THROW_IF( idx>=getChildrenCount(), "JsonSchema", 10,  "array index " + std::to_string(idx) + " is out of range" );
    return *children[idx];
}

JsonSchema& JsonSchema::get_child(std::size_t idx)
{
    JSONIO_THROW_IF( idx>getChildrenCount(), "JsonSchema", 11, "array index " + std::to_string(idx) + " is out of range" );
    if( idx==getChildrenCount() ) // next element
    {
        auto obj = dynamic_cast<JsonSchema*>(append_node( std::to_string(idx), JsonBase::Null, "" ));
        JSONIO_THROW_IF( obj==nullptr, "JsonSchema", 12, "array element " + std::to_string(idx) + " is illegal type" );
        return *obj;
    }
    return *children[idx];
}

JsonSchema& JsonSchema::get_child(const std::string &key)
{
    auto element = find_key(key);
    if( element == children.end() )
    {
        auto obj = dynamic_cast<JsonSchema*>(append_node( key, JsonBase::Null, "" ));
        JSONIO_THROW_IF( obj==nullptr, "JsonSchema", 13, "object element " + key + " is illegal type" );
        return *obj;
    }
    return *element->get();
}

const JsonSchema& JsonSchema::get_child(const std::string &key) const
{
    auto element = find_key(key);
    if( element == children.end() )
    {
        JSONIO_THROW( "JsonSchema", 14, "key '" + key + "' not found" );
    }
    return *element->get();
}

JsonSchema &JsonSchema::get_parent() const
{
    JSONIO_THROW_IF( !parent_object, "JsonSchema", 15, "parent object is undefined" );
    return *parent_object;
}

bool JsonSchema::remove()
{
    if( parent_object == nullptr )
        return false;
    return parent_object->remove_child( this );
}

bool JsonSchema::remove_child( JsonSchema* child )
{
    if( level_type==0 && child->field_descrip->required() == FieldDef::fld_required )
    {
        // not remove requered field ( only clear )
        child->clear();
        return true;
    }

    int thisndx = -1;
    for(std::size_t ii=0; ii< children.size(); ii++ )
    {
        if( children[ii].get() == child )
            thisndx = static_cast<int>(ii);
        if( thisndx >= 0 )  {
          children[ii]->ndx_in_parent--;
          if( isArray() )
              children[ii]->field_key = std::to_string(children[ii]->ndx_in_parent);
        }
    }
    if( thisndx >= 0 )
    {
        children.erase(children.begin() + thisndx);
        return true;
    }
    return false;
}

bool JsonSchema::remove_child( std::size_t idx )
{
    return remove_child( &get_child( idx ) );
}

bool JsonSchema::remove_child( const std::string &key )
{
    return remove_child( &get_child( key ) );
}

JsonSchema *JsonSchema::field( std::queue<std::string> names ) const
{
    if( names.empty() )
        return const_cast<JsonSchema *>(this);

    auto fname = names.front();
    names.pop();

    auto element = find_key(fname);
    if( element == children.end() )
    {
        return nullptr;
    }
    return element->get()->field(names);
}

JsonSchema *JsonSchema::field( std::queue<int> ids ) const
{
    if( ids.empty() )
        return  const_cast<JsonSchema *>(this);

    int aid = ids.front();
    ids.pop();

    for(size_t ii=0; ii< children.size(); ii++ )
        if( ( !isArray() && !isMap() && children[ii]->field_descrip->id() == aid ) ||
            ( (isArray() || isMap()) && static_cast<int>(ii) == aid )    )
            return children[ii]->field( ids );

    return nullptr;  // not found
}

const JsonSchema *JsonSchema::field(const std::string &fieldpath) const
{
    auto pos = fieldpath.find_first_not_of("0123456789.");
    if( pos != std::string::npos || isArray() )
    {
        auto names = split( fieldpath, field_path_delimiters );
        return field(names);
    }
    else
    {
        auto ids = split_int( fieldpath, ".");
        return field(ids);
    }
}

JsonSchema &JsonSchema::add_object_via_path(const std::string &jsonpath)
{
    auto names = split(jsonpath, field_path_delimiters);
    auto pobj = field_add_names( names );
    if( pobj )
    {
        if( !pobj->isObject())
            pobj->update_node( JsonBase::Object, "" );
        return *pobj;
    }
    JSONIO_THROW( "JsonBase", 16, "cannot create object with jsonpath " + std::string( jsonpath ) );

}

JsonSchema &JsonSchema::add_array_via_path(const std::string &jsonpath)
{
    auto names = split(jsonpath, field_path_delimiters);
    auto pobj = field_add_names( names );
    if( pobj )
    {
        if( !pobj->isArray())
            pobj->update_node( JsonBase::Array, "" );
        return *pobj;
    }
    JSONIO_THROW( "JsonBase", 17, "cannot create array with jsonpath " + std::string( jsonpath ) );

}

JsonSchema *JsonSchema::field_add_names( std::queue<std::string> names )
{
    if( names.empty() )
        return const_cast<JsonSchema *>(this);

    auto fname = names.front();
    names.pop();

    auto element = find_key(fname);
    if( element == children.end() )
    {
        if( isObject() )
        {
            auto new_obj = append_node( fname, JsonBase::Null, "" );
            return /*children.back()*/ new_obj->field_add_names(names);
        }
        else if( isArray() && fname== std::to_string(children.size()) )
        {
            append_node( fname, JsonBase::Null, "" );
            return children.back()->field_add_names(names);
        }
        else
            return nullptr;
    }
    return element->get()->field_add_names(names);
}

list_names_t JsonSchema::getUsedKeys() const
{
    list_names_t usekeys;
    for( const auto& el: children )
    {
        usekeys.push_back( el->getKey() );
    }
    return usekeys;
}

list_names_t JsonSchema::getNoUsedKeys() const
{
    list_names_t lst;
    if( isStruct() )
    {
        auto use_fields = getUsedKeys();
        auto all_fields = struct_descrip->getFields(false);
        for( const auto& field: all_fields )
        {
            if( std::find( use_fields.begin(), use_fields.end(), field ) == use_fields.end() )
                lst.push_back( field);
        }
    }
    return lst;
}

std::string JsonSchema::getHelpName() const
{
    return struct_descrip->name() + "#"+ JsonBase::getHelpName();
}

std::string JsonSchema::getDescription() const
{
    if( level_type == 0 )
        return  field_descrip->description();
    return "";
}

JsonSchema *JsonSchema::field_add(const std::string &fieldpath)
{
    auto names = split(fieldpath, field_path_delimiters);
    return field_add_names(names);
}

std::string JsonSchema::getFullDescription() const
{
    // get doc from thrift schema
    if( level_type == 0 )
        return  getDescription();

    auto desc = parent_object->getFullDescription();
    desc += " ";
    desc += getKey();
    return desc;
}

JsonSchema *JsonSchema::getChild(const std::string &key) const
{
    auto element = find_key(key);
    if( element == children.end() )
    {
        return nullptr;
    }
    return element->get();
}

bool JsonSchema::clear()
{
    set_children();
    set_default_value();
    return true;
}

std::vector<size_t> JsonSchema::array_sizes() const
{
    std::vector<size_t> sizes;

    if( !( isArray() || isMap() ) )
        return sizes;

    if( !empty() )
    {
        sizes = children[0]->array_sizes();
    }

    sizes.insert( sizes.begin(), size() );
    return sizes;
}

void JsonSchema::resize_array_level( size_t level, const std::vector<size_t>& sizes, const std::string& defval  )
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

void JsonSchema::array_resize( std::size_t  newsize, const std::string& defval  )
{
    JSONIO_THROW_IF( !(isArray() || isMap() ), "JsonSchema", 18, "cannot resize not array data " + std::string( typeName() ) );

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
                children.emplace_back( std::make_shared<JsonSchema>(*defchild) );
                children.back()->field_key = std::to_string(ii);
                children.back()->ndx_in_parent = ii;
                children.back()->parent_object = this;

                if( !defval.empty() && defchild->isPrimitive() )
                    children.back()->field_value = chdefval;
            }
        }
        else
        {
            auto level = level_type;
            if( isMap() )
              level++;
            level++;
            auto new_type = fieldtype2basetype( field_descrip->type(level) );
            std::string new_el_value = checked_value(new_type, defval );

            for( auto ii=children.size(); ii<newsize; ii++)
                append_node( std::to_string(ii),  new_type, new_el_value );
        }
    }
}


//----------------------------------------------------------------------------------------------

JsonBase::Type JsonSchema::fieldtype2basetype( FieldDef::FieldType field_type )
{
    JsonBase::Type base_type = JsonBase::Null;
    switch( field_type )
    {
    case FieldDef::T_STOP:
    case FieldDef::T_VOID:
        base_type = JsonBase::Null;
        break;
    case FieldDef::T_BOOL:
        base_type = JsonBase::Bool;
        break;
    case FieldDef::T_I08:
    case FieldDef::T_I16:
    case FieldDef::T_I32:
        base_type = JsonBase::Int;
        break;
    case FieldDef::T_U64:
    case FieldDef::T_I64:
    case FieldDef::T_DOUBLE:
        base_type = JsonBase::Double;
        break;
    case FieldDef::T_UTF16:
    case FieldDef::T_UTF8:
    case FieldDef::T_STRING:
        base_type = JsonBase::String;
        break;
    case FieldDef::T_STRUCT:
    case FieldDef::T_MAP:
        base_type = JsonBase::Object;
        break;
    case FieldDef::T_SET:
    case FieldDef::T_LIST:
        base_type = JsonBase::Array;
        break;
    }
    return base_type;
}

void JsonSchema::set_children()
{
    children.clear();

    // add levels for objects
    if( isStruct() )
    {
        const  StructDef* strDef2;
        if( isTop() )
            strDef2  = struct_descrip;
        else
            strDef2  = ioSettings().Schema().getStruct( field_descrip->className() );

        JSONIO_THROW_IF( strDef2 == nullptr,"JsonSchema", 19, "undefined struct definition " + field_descrip->className()  );
        struct2model( strDef2 );
    }
}

void JsonSchema::struct2model( const  StructDef* strDef )
{
    struct_descrip = strDef;
    auto it = struct_descrip->cbegin();
    while( it != struct_descrip->cend() )
    {
        if( it->get()->required() == FieldDef::fld_required ||
                !it->get()->defaultValue().empty() ||
                it->get()->className() == "TimeStamp" )
        {
            // add only requrled
            auto child = new JsonSchema( it->get(), this );
            children.push_back( std::shared_ptr<JsonSchema>(child) );
        }
        it++;
    }
}

void JsonSchema::set_value(JsonBase::Type /*atype*/, const std::string& value )
{
    //add_setup();
    // ??? check before // auto avalue = checked_value( type(), value );
    field_value = value;
}

// Set up current time as default value
// only for field
void JsonSchema::set_current_time()
{
    if(field_descrip->className() != "TimeStamp")
        return;

    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    set_value_via_path( "year", 1900+time_now->tm_year );
    set_value_via_path( "month", time_now->tm_mon+1 );
    set_value_via_path( "day", time_now->tm_mday );
    set_value_via_path( "hour", time_now->tm_hour );
    set_value_via_path( "minute", time_now->tm_min );
    set_value_via_path( "second", time_now->tm_sec );
}

} // namespace jsonio
