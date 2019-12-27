#pragma once

#include <memory>
#include <map>
#include "jsonio14/jsonbase.h"


namespace jsonio14 {


/// Definition of one field in thrift structure
/// [id:] [requiredness] type FieldName [=default] [,|;]
class FieldDef
{

public:

    /// #include <thrift/protocol/TVirtualProtocol.h>
    /// Enumerated definition of the types that the Thrift protocol supports.
    enum FieldType {
        Th_STOP       = 0,
        Th_VOID       = 1,
        Th_BOOL       = 2,
        Th_BYTE       = 3,
        Th_I08        = 3,
        Th_I16        = 6,
        Th_I32        = 8,
        Th_U64        = 9,
        Th_I64        = 10,
        Th_DOUBLE     = 4,
        Th_STRING     = 11,
        Th_UTF7       = 11,
        Th_STRUCT     = 12,
        Th_MAP        = 13,
        Th_SET        = 14,
        Th_LIST       = 15,
        Th_UTF8       = 16,
        Th_UTF16      = 17
    };


    /// A requiredness attribute which defines how
    /// Apache Thrift reads and writes the field
    enum  FieldRequered
    {
        fld_required = 0,
        fld_default = 1,
        fld_optional = 2
    };

    /// Field id
    virtual int id() const = 0;

    /// Field Name
    virtual std::string name() const = 0;

    /// Field Type or all "typeId"+"type"+"elemTypeId"+"elemTypeId"  - all levels
    virtual FieldType type( int level=0 ) const = 0;

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

/// Thrift structs definition
class StructDef
{

public:

    /// Get name of structure
    const char* name() const {
        return schema_name.c_str();
    }

    /// Get description of structure
    const char* description() const {
        return schema_description.c_str();
    }

    /// Test structure is Union
    bool isUnion() const {
        return is_union;
    }

    /// Get unique fields list
    const list_names_t& getSelectedList() const {
        return key_id_list;
    }

    /// Get unique fields list
    const list_names_t& getUniqueList() const {
        return unique_list;
    }

    /// Get key template fields list
    const list_names_t& getKeyTemplateList() const {
        return key_template_list;
    }

    /// Get field definition by name
    const FieldDef* getField( const std::string& fName ) const
    {
        auto it = name2index.find( fName );
        if( it == name2index.end() )
            return nullptr;
        else
            return fields[it->second].get();
    }

    /// Get field definition by index
    const FieldDef* getField( int fId ) const
    {
        auto it = id2index.find( fId );
        if( it == id2index.end() )
            return nullptr;
        else
            return fields[it->second].get();
    }

private:

    std::string schema_name;                    ///< Name of strucure
    bool is_union;                              ///< True if union
    std::string  schema_description;            ///< Comment

    std::vector< std::shared_ptr<FieldDef>> fields;   ///< List of fields

    std::map<int, size_t> id2index;
    std::map<std::string, size_t> name2index;

    list_names_t  key_id_list;
    list_names_t  key_template_list;
    list_names_t  unique_list;

};

/// Thrift enums definition
class EnumDef
{

public:

    const static int empty_enum = std::numeric_limits<int>::min();

    /// Get name of enum
    const char* name() const {
        return enum_name.c_str();
    }

    /// Get description of structure
    const char* description() const {
        return enum_sdoc.c_str();
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

private:

    std::string  enum_name;                       ///< Name of enum
    std::string  enum_sdoc;                      ///< Comment
    std::map<std::string, int> name2index;       ///< Members of enum
    std::map<int, std::string> index2name;       ///< Members of enum
    std::map<std::string, std::string> name2doc; ///< Comments of enum

};

/// Thrift schema definition
class SchemasData
{

public:

    /// Read thrift schema from json file fileName
    virtual void addSchemaFile( const std::string& file_path ) = 0;


    /// Get description of structure
    StructDef* getStruct(const std::string& struct_name) const
    {
        auto it = structs.find( struct_name );
        if( it == structs.end() )
            return nullptr;
        else
            return it->second.get();
    }

    /// Get description of enum
    EnumDef* getEnum(const std::string& enum_name) const
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

private:

    std::map<std::string, std::string> files; ///< List of readed files (name, doc)

    std::map<std::string, std::shared_ptr<StructDef>> structs; ///< List of all structures
    std::map<std::string, std::shared_ptr<EnumDef>> enums;     ///< List of all enums

};

} // namespace jsonio14


