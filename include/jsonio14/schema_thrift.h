#pragma once

#include <memory>
#include <map>
#include "jsonio14/schema.h"
#include "jsonio14/jsonfree.h"

namespace jsonio14 {


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
    virtual FieldType type( int level=0 ) const override {
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

    int f_id = -1;                         ///< Key id
    std::string f_name = "";               ///< Field Name
    std::vector<FieldType> f_type_id; ///< Type or all "typeId"+"type"+"elemTypeId"+"elemTypeId"  - all levels
    FieldRequered f_required = fld_default;         ///< A requiredness attribute (REQURED_FIELD)
    std::string  f_default ="";           ///< Default value
    std::string  inserted_default="";    ///< Default value from editor
    std::string  class_name="";          ///< "class": Struct or enum name for corresponding field type
    std::string  f_doc="";               ///< "doc" - Comment string
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

    /// Constructor - read information from json/bson schema
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
    ThriftEnumDef( const JsonFree& object )
    {
        readEnum( object );
    }
};

/// Thrift schema definition
class ThriftSchema : public SchemasData
{

    void readSchema( const JsonFree& object );

public:


    /// ThriftSchema - constructor
    ThriftSchema():SchemasData()
    {
        ThriftFieldDef::setTypeMap();
    }

    /// Read thrift schema from json file fileName
    void addSchemaFile( const std::string& file_path ) override;
};

} // namespace jsonio14


