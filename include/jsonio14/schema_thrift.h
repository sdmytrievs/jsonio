/// Implementation of FieldDef, StructDef, EnumDef interfaces for Thrift schemas

#pragma once
#include "jsonio14/schema.h"

namespace jsonio14 {

const std::string schema_thrift = "thrift";

/// Definition of one field in thrift structure
/// [id:] [requiredness] type FieldName [=default] [,|;]
class ThriftFieldDef : public FieldDef
{

public:

    static void setTypeMap();

    /// Constructor - read information from json/bson schema
    ThriftFieldDef( const JsonFree& object ):
        FieldDef(), f_type_id()
    {
        readField( object );
    }

    /// Field id
    virtual int id() const override {
        return f_id;
    }
    /// Field Name
    virtual std::string name() const override {
        return f_name;
    }
    /// Field Type or all "typeId"+"type"+"elemTypeId"+"elemTypeId"  - all levels
    virtual FieldType type( size_t level=0 ) const override {
        JARANGO_THROW_IF( level >= f_type_id.size(), "ThriftSchema", 2, "undefined field level " );
        return f_type_id[level];
    }
    /// A requiredness attribute (FieldRequered)
    virtual FieldRequered required() const override {
        return f_required;
    }
    /// "doc" - Comment string
    virtual std::string  description() const override {
        return f_doc;
    }
    /// "class": Struct or enum name for corresponding field type
    virtual std::string  className()const override {
        return class_name;
    }

    /// Default value
    virtual std::string  defaultValue()const override {
        return f_default;
    }
    virtual double minValue()const override {
        return minval;
    }
    virtual double maxValue()const override {
        return maxval;
    }

protected:

    static std::map<std::string, FieldType> name_to_thrift_types;

    /// Field id
    int f_id = -1;
    /// Field Name
    std::string f_name = "";
    /// "doc" - Comment string
    std::string  f_doc="";
    /// Field Type or all "typeId"+"type"+"elemTypeId"+"elemTypeId"
    std::vector<FieldType> f_type_id;
    /// A requiredness attribute (REQURED_FIELD)
    FieldRequered f_required = fld_default;
    /// Default value ( json string for struct or array )
    std::string  f_default ="";
    /// Default value from editor
    std::string  inserted_default="";
    /// "class": Struct or enum name for corresponding field type
    std::string  class_name="";

    double minval = std::numeric_limits<double>::min();
    double maxval = std::numeric_limits<double>::max();

    void readField( const JsonFree& object );
    void read_type_spec( const JsonFree& object, const char* keyspec, const std::string& typeID);
};

/// Thrift structs definition
class ThriftStructDef: public StructDef
{
    bool is_exception = false;
    void readSchema( const JsonFree& object );

public:

    /// Constructor - read information from json schema
    ThriftStructDef( const JsonFree& object ): StructDef()
    {
        readSchema( object );
    }

};

/// Thrift enums definition
class ThriftEnumDef: public EnumDef
{

    void readEnum( const JsonFree& object );

public:

    /// ThriftEnumDef - read information from json schema
    ThriftEnumDef( const JsonFree& object ): EnumDef()
    {
        readEnum( object );
    }

};

/// Thrift schema definition
/// Read thrift schema from json string
void ThriftSchemaRead( const std::string& jsondata, schema_files_t& files,
                                  schemas_t& structs,  enums_t& enums );

} // namespace jsonio14


