#pragma once

#include <map>
#include <memory>
#include <functional>
#include "jsonio17/txt2file.h"


namespace jsonio17 {


/// Definition of one field in structure
/// [id:] [requiredness] type FieldName [=default] [,|;]
class FieldDef
{

protected:

    static std::shared_ptr<FieldDef> top_field;

public:

    static const FieldDef* topfield()
    {
        return  top_field.get();
    }


    /// #include <thrift/protocol/TVirtualProtocol.h>
    /// Enumerated definition of the types that the Thrift protocol supports.
    enum FieldType {
        T_STOP       = 0,
        T_VOID       = 1,
        T_BOOL       = 2,
        T_BYTE       = 3,
        T_I08        = 3,
        T_I16        = 6,
        T_I32        = 8,
        T_U64        = 9,
        T_I64        = 10,
        T_DOUBLE     = 4,
        T_STRING     = 11,
        T_UTF7       = 11,
        T_STRUCT     = 12,
        T_MAP        = 13,
        T_SET        = 14,
        T_LIST       = 15,
        T_UTF8       = 16,
        T_UTF16      = 17
    };


    /// A requiredness attribute which defines how
    /// Apache Thrift reads and writes the field
    enum  FieldRequered
    {
        fld_required = 0,
        fld_default = 1,
        fld_optional = 2
    };

    /// Destructor
    virtual ~FieldDef() = default;

    /// Field id
    virtual int id() const = 0;

    /// Field Name
    virtual std::string name() const = 0;

    /// Field Type or all "typeId"+"type"+"elemTypeId"+"elemTypeId"  - all levels
    virtual FieldType type( size_t level=0 ) const = 0;

    /// Field Type for back level
    virtual FieldType elemType() const = 0;

    /// A requiredness attribute (FieldRequered)
    virtual FieldRequered required() const = 0;

    /// "doc" - Comment string
    virtual std::string  description() const = 0;

    /// "class": Struct or enum name for corresponding field type
    virtual std::string  className() const = 0;

    /// Default value
    virtual std::string  defaultValue() const = 0;
    virtual double minValue() const = 0;
    virtual double maxValue() const = 0;

};

/// Structured data definition
class StructDef
{

public:

    /// Destructor
    virtual ~StructDef() = default;

    /// A const iterator for a fields of structure
    using const_iterator = std::vector<std::shared_ptr<FieldDef>>::const_iterator;

    /// @brief returns a const iterator to one past the last field.
    const_iterator cend() const
    {
        return fields.cend();
    }

    /// @brief returns a const iterator to the first field
    const_iterator cbegin() const
    {
        return fields.cbegin();
    }

    /// Get size of structure
    size_t size() const {
        return fields.size();
    }

    /// Get name of structure
    const std::string& name() const {
        return schema_name;
    }

    /// Get description of structure
    const std::string& description() const {
        return schema_description;
    }

    /// Test structure is Union
    bool isUnion() const {
        return is_union;
    }

    /// Get fields list used to generate default selection table
    const list_names_t& getSelectedList() const {
        return key_select_list;
    }

    /// Generate field name from ids list
    std::string getPathFromIds( std::queue<int> ids ) const;

    /// Get list of fields what must to be unique
    const list_names_t& getUniqueList() const {
        return unique_list;
    }

    /// Get fields list to be used to generate default document id
    const list_names_t& getKeyTemplateList() const {
        return key_template_list;
    }

    /// Get field definition by name
    const  FieldDef* getField( const std::string& fName ) const
    {
        auto it = name2index.find( fName );
        if( it == name2index.end() )
            return nullptr;
        else
            return fields[it->second].get();
    }

    /// Get field definition by index
    const  FieldDef* getField( int fId ) const
    {
        auto it = id2index.find( fId );
        if( it == id2index.end() )
            return nullptr;
        else
            return fields[it->second].get();
    }

    /// Get fields names list
    list_names_t getFields( bool with_doc ) const
    {
        list_names_t members;
        auto it = fields.begin();
        while( it != fields.end() )
        {
            auto vasl = it->get()->name();
            if( with_doc  )
            {
                vasl+= ',';
                vasl+= it->get()->description();
            }
            members.push_back(vasl);
            it++;
        }
        return members;
    }

    /// Get fields names set before fname in structure
    set_names_t getFieldsBefore( const std::string& fname ) const
    {
        set_names_t members;
        auto it = fields.begin();
        while( it != fields.end() )
        {
            auto vasl = it->get()->name();
            if( vasl == fname  )
            {
                break;
            }
            members.insert(vasl);
            it++;
        }
        return members;
    }

protected:

    /// Name of strucure
    std::string schema_name = "";
    /// Comment/description
    std::string  schema_description = "";
    /// True if union
    bool is_union = false;

    /// List of fields in structure
    std::vector< std::shared_ptr<FieldDef>> fields = {};

    /// Map field definition by index
    std::map<int, size_t> id2index ={};
    /// Map field definition by name
    std::map<std::string, size_t> name2index ={};

    /// Fields list to default selection
    list_names_t  key_select_list ={};
    /// Fields list to be used to generate default document id
    list_names_t  key_template_list ={};
    /// Combinations of fields what must to be unique
    list_names_t  unique_list ={};

};

/// Enum data definition
class EnumDef
{

public:

    static int empty_enum;

    /// Destructor
    virtual ~EnumDef() = default;

    /// Get name of enum
    const std::string& name() const {
        return enum_name;
    }

    /// Get description of structure
    const std::string& description() const {
        return enum_sdoc;
    }

    /// Get enum name by value
    std::string value2name( int value ) const
    {
        auto it = index2name.find( value );
        if( it == index2name.end() )
            return "";
        else
            return it->second;
    }

    /// Get enum value by name
    int name2value( const std::string& line_name ) const
    {
        auto it = name2index.find( line_name );
        if( it == name2index.end() )
            return empty_enum;
        else
            return it->second;
    }

    /// Get enum comment by name
    std::string name2description( const std::string& line_name ) const
    {
        auto it = name2doc.find( line_name );
        if( it == name2doc.end() )
            return "";
        else
            return it->second;
    }

    /// Get enum names list
    list_names_t all_names() const
    {
        list_names_t members;

        auto it = name2index.begin();
        while( it != name2index.end() )
            members.push_back(it++->first);
        return members;
    }

protected:

    /// Name of enum
    std::string  enum_name ="";
    /// Comment/description of enum
    std::string  enum_sdoc ="";
    /// Map enum name to value
    std::map<std::string, int> name2index ={};
    /// Map enum value to name
    std::map<int, std::string> index2name ={};
    /// Map enum name to description
    std::map<std::string, std::string> name2doc ={};

};

using schema_files_t = std::map<std::string, std::string>;
using schemas_t = std::map<std::string, std::shared_ptr<StructDef>>;
using enums_t = std::map<std::string, std::shared_ptr<EnumDef>>;
/// Factory method fetching schema definition from a json format strin
using  SchemaReadFactory_f = std::function<void( const std::string& jsondata,
schema_files_t& files, schemas_t& schemas,  enums_t& enums )>;


///  All json schemas collection
class SchemasData final
{

public:

    /// Registering new schemas format and Factory Method where instances are actually created
    void addSchemaMethod( const std::string& schema_type, SchemaReadFactory_f method )
    {
        methods[schema_type] = method;
    }

    /// Read schema description from json file file_path
    void addSchemaFile( const std::string& format_type, const std::string& file_path )
    {
        auto json_data = read_ascii_file( file_path );
        addSchemaFormat( format_type, json_data );
    }

    /// Read schema description from json string
    void addSchemaFormat( const std::string& format_type, const std::string& json_string )
    {
        if(json_string.empty())
            return;
        auto read_method = methods.find(format_type);
        if( read_method != methods.end() )
            read_method->second(json_string, files, structs, enums);
    }

    /// Get description of structure
    const StructDef* getStruct(const std::string& struct_name) const
    {
        auto it = structs.find( struct_name );
        if( it == structs.end() )
            return nullptr;
        else
            return it->second.get();
    }

    /// Get description of enum
    const EnumDef* getEnum(const std::string& enum_name) const
    {
        auto it = enums.find( enum_name );
        if( it == enums.end() )
            return nullptr;
        else
            return it->second.get();
    }

    /// Get structs names list
    list_names_t getStructs( bool with_doc ) const
    {
        list_names_t members;
        auto it = structs.begin();
        while( it != structs.end() )
        {
            auto vasl = it->first;
            if( with_doc  )
            {
                vasl+= ',';
                vasl+= it->second->description();
            }
            members.push_back(vasl);
            it++;
        }
        return members;
    }

    /// Get enums names list
    list_names_t getEnums() const
    {
        list_names_t members;

        auto it = enums.begin();
        while( it != enums.end() )
            members.push_back(it++->first);
        return members;
    }

    /// Clear all internal data
    void clear_all()
    {
        files.clear();
        structs.clear();
        enums.clear();
    }

    const schemas_t& allStructs() const
    {
        return structs;
    }

protected:

    /// Dictionary of readed files (name, doc)
    schema_files_t files = {};
    /// Dictionary of all structures
    schemas_t structs = {};
    /// Dictionary of all enums
    enums_t enums = {};

    /// Dictionary of implemented schemas  for Factory Method
    std::map<std::string, SchemaReadFactory_f> methods = {};
};

} // namespace jsonio17


