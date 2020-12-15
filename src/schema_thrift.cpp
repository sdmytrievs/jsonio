
#include "jsonio17/schema_thrift.h"
#include "jsonio17/service.h"
#include "jsonio17/jsondump.h"
#include "jsonio17/io_settings.h"

namespace jsonio17 {


// Generate field name from ids list
std::string StructDef::getPathFromIds( std::queue<int> ids ) const
{
    std::string key_name;
    if( ids.empty() )
        return  key_name;

    int aid = ids.front();
    ids.pop();

    auto field_def = getField( aid );
    if( field_def == nullptr )
        return key_name;

    key_name = field_def->name();
    int level = 0;
    while(   field_def->type(level) == jsonio17::FieldDef::T_SET ||
             field_def->type(level) == jsonio17::FieldDef::T_LIST ||
             field_def->type(level) == jsonio17::FieldDef::T_MAP  )
    {
        if( ids.empty() )
            return key_name;

        aid = ids.front();
        ids.pop();
        key_name += "."+ std::to_string(aid);
        if(field_def->type(level) == jsonio17::FieldDef::T_MAP )
            level++;
        level++;
    }

    if( !ids.empty() && field_def->type(level) == jsonio17::FieldDef::T_STRUCT )
    {
        auto filed_str  = ioSettings().Schema().getStruct( field_def->className() );
        if( filed_str != nullptr )
        {
            auto next_path = filed_str->getPathFromIds( ids );
            if (!next_path.empty() )
                key_name += "." + next_path;
        }
    }
    return key_name;
}

static const char * key_name = "name";
static const char * key_doc = "doc";
static const char * key_fields ="fields";
static const char * key_isException ="isException";
static const char * key_isUnion ="isUnion";
static const char * key_toselect ="to_select";
static const char * key_tokeytemplate ="to_key";
static const char * key_unique_fields ="to_unique";

static const char * key_key ="key";
static const char * key_typeId = "typeId";
static const char * key_type = "type";
static const char * key_elemTypeId = "elemTypeId";
static const char * key_elemType = "elemType";
static const char * key_keyTypeId ="keyTypeId";
static const char * key_keyType ="keyType";
static const char * key_valueTypeId ="valueTypeId";
static const char * key_valueType ="valueType";
static const char * key_default ="default";
static const char * key_minval ="minval";
static const char * key_maxval ="maxval";
static const char * key_class ="class";
static const char * key_required ="required";
const char * key_req_out ="req_out";
static const char * key_optional ="optional";
static const std::string key_undefined ="undefined";

static const char * key_members ="members";
static const char * key_value ="value";
static const char * key_structs ="structs";
static const char * key_enums ="enums";


std::shared_ptr<FieldDef> FieldDef::top_field = std::make_shared<ThriftFieldDef>(
            1, "top", "Default top field", std::vector<FieldDef::FieldType>( { FieldDef::T_STRUCT } ));

std::map<std::string, ThriftFieldDef::FieldType> ThriftFieldDef::name_to_thrift_types;
int EnumDef::empty_enum = std::numeric_limits<int>::min();

void ThriftFieldDef::set_type_map()
{
    if( !name_to_thrift_types.empty())
        return;

    name_to_thrift_types["stop"]= FieldDef::T_STOP;
    name_to_thrift_types["void"]= FieldDef::T_VOID;
    name_to_thrift_types["bool"]= FieldDef::T_BOOL;
    name_to_thrift_types["byte"]= FieldDef::T_BYTE;
    name_to_thrift_types["i8"]= FieldDef::T_I08;
    name_to_thrift_types["i16"]= FieldDef::T_I16;
    name_to_thrift_types["i32"]= FieldDef::T_I32;
    name_to_thrift_types["u64"]= FieldDef::T_U64;
    name_to_thrift_types["i64"]= FieldDef::T_I64 ;
    name_to_thrift_types["double"]= FieldDef::T_DOUBLE;
    name_to_thrift_types["string"]= FieldDef::T_STRING;
    name_to_thrift_types["utf7"]= FieldDef::T_UTF7;
    name_to_thrift_types["struct"]= FieldDef::T_STRUCT;
    name_to_thrift_types["union"]= FieldDef::T_STRUCT;
    name_to_thrift_types["exception"]= FieldDef::T_STRUCT;
    name_to_thrift_types["map"]= FieldDef::T_MAP;
    name_to_thrift_types["set"]= FieldDef::T_SET;
    name_to_thrift_types["list"]= FieldDef::T_LIST;
    name_to_thrift_types["utf8"]= FieldDef::T_UTF8;
    name_to_thrift_types["utf16"]= FieldDef::T_UTF16;
}


/// Read json schema for one thrift structure from bsondata
void ThriftFieldDef::read_field( const JsonFree& field_object )
{
    field_object.get_value_via_path( key_key, f_id, -1 );
    field_object.get_value_via_path( key_name, f_name, key_undefined );
    field_object.get_value_via_path<std::string>( key_doc, f_doc, "" );
    replace_all(f_doc, "\t\r\n", ' ' );
    field_object.get_value_via_path<std::string>( key_class, class_name,  "");   // "class": enum name
    // default value we get to string for different types
    field_object.get_value_via_path<std::string>( key_default, f_default, "" );

    field_object.get_value_via_path( key_minval, minval, std::numeric_limits<double>::min() );
    field_object.get_value_via_path( key_maxval, maxval, std::numeric_limits<double>::max() );

    std::string requred;
    field_object.get_value_via_path<std::string>( key_required, requred, key_req_out);
    if( requred == key_required )
        f_required = fld_required;
    else if( requred == key_optional )
        f_required = fld_optional;
    else
        f_required = fld_default;

    // vector<int> fTypeId; // "typeId"+"type"+"elemTypeId"+"elemType"  - all levels
    std::string  typeId;
    if( !field_object.get_value_via_path<std::string>(  key_typeId, typeId, ""  ) )
        f_type_id.push_back(T_VOID);
    else
        read_type_spec(field_object, key_type, typeId);

    if( f_type_id[0] == T_STRING and !f_default.empty() )
       f_default = "\""+f_default+"\"";
}

/// Read json schema for thrift structure from bsondata
void ThriftFieldDef::read_type_spec( const JsonFree& field_object,
                                     const char* keyspec, const std::string& typeID)
{
    auto it = name_to_thrift_types.find(typeID);
    if( it == name_to_thrift_types.end() )
        JSONIO_THROW( "ThriftSchema", 1, "undefined field type " + typeID  );

    auto type = it->second;
    f_type_id.push_back(type);

    if( type == T_STRUCT )
    {
        auto nextobj = field_object[ keyspec ];
        if( !nextobj.get_value_via_path<std::string>(  key_class, class_name, "" ) )
            JSONIO_THROW( "ThriftSchema", 2, "undefined class name "+typeID );
        size_t pos = class_name.find_last_of('.');
        if( pos != std::string::npos)
            class_name.erase(0, pos+1);
    }
    else if ( type == T_MAP)
    {
        auto nextobj = field_object[ keyspec ];
        std::string  newtypeId;
        if( !nextobj.get_value_via_path<std::string>(  key_keyTypeId, newtypeId, ""  ) )
            f_type_id.push_back(T_VOID);
        else
            read_type_spec(nextobj, key_keyType, newtypeId);

        if( !nextobj.get_value_via_path<std::string>(  key_valueTypeId, newtypeId, ""  ) )
            f_type_id.push_back(T_VOID);
        else
            read_type_spec(nextobj, key_valueType, newtypeId);

    }
    else  if (type == T_LIST || type == T_SET)
    {
        auto nextobj = field_object[ keyspec ];
        std::string  newtypeId;
        if( !nextobj.get_value_via_path<std::string>(  key_elemTypeId, newtypeId, ""  ) )
            f_type_id.push_back(T_VOID);
        else
            read_type_spec(nextobj, key_elemType, newtypeId);
    }
}


/// Read json schema for one thrift structure from bsondata
void ThriftStructDef::read_schema( const JsonFree& object )
{
    // read information about structure
    object.get_value_via_path( key_name, schema_name, key_undefined );
    object.get_value_via_path( key_isException, is_exception, false );
    object.get_value_via_path( key_isUnion, is_union, false );
    object.get_value_via_path<std::string>( key_doc, schema_description, "" );
    replace_all( schema_description, "\t\r\n", ' ' );

    // read to_select
    object.get_value_via_path( key_toselect, key_select_list, {} );
    // read to_key
    object.get_value_via_path( key_tokeytemplate, key_template_list, {} );
    // read unique_fields
    object.get_value_via_path( key_unique_fields, unique_list, {} );

    // read arrays of fields
    auto fieldsarr  = object[key_fields];
    size_t ii = 0;
    for ( const auto& element : fieldsarr )
    {
        auto new_field = std::make_shared<ThriftFieldDef>(*element);
        fields.push_back( new_field );
        id2index[new_field->id()] = ii;
        name2index[new_field->name()] = ii++;
    }
}


/// Read json schema for one thrift enum from bsondata
void ThriftEnumDef::read_enum( const JsonFree& object )
{
    // read information about enum
    object.get_value_via_path( key_name, enum_name,  key_undefined);  // May be exeption
    object.get_value_via_path<std::string>( key_doc, enum_sdoc, "" );
    replace_all(enum_sdoc, "\t\r\n", ' ' );

    // read arrays of fields
    std::string ename, edoc;
    int eId;
    auto fieldsarr  =  object[ key_members ];
    for( const auto& element : fieldsarr )
    {
        element->get_value_via_path( key_name, ename , key_undefined);
        element->get_value_via_path<std::string>( key_doc, edoc, "" );
        replace_all(edoc, "\t\r\n", ' ' );
        element->get_value_via_path( key_value, eId, empty_enum );

        name2index[ ename ] = eId;
        index2name[ eId ] = ename;
        name2doc[ ename ] =  edoc;
    }
}

//-----------------------------------------------------------

// Read thrift schema from json string
void ThriftSchemaRead( const std::string& jsondata, schema_files_t& files,
                       schemas_t& structs,  enums_t& enums )
{
    ThriftFieldDef::set_type_map();
    auto object =  json::loads( jsondata );

    // extract schema to internal structures
    std::string fname, fdoc;
    object.get_value_via_path( key_name, fname , key_undefined);
    object.get_value_via_path<std::string>( key_doc, fdoc, "" );
    files[ fname ] = fdoc;

    // read enum array
    auto enumsarr  = object[ key_enums ];
    for( const auto& enumobject : enumsarr )
    {
        auto enumdata = std::make_shared<ThriftEnumDef>(*enumobject);
        enums[ enumdata->name() ] = enumdata;
    }

    // read structs array
    auto structsarr  = object[ key_structs ];
    for( const auto& structobject : structsarr )
    {
        auto structdata = std::make_shared<ThriftStructDef>(*structobject);
        structs[ structdata->name() ] = structdata;
    }
}


} // namespace jsonio17
