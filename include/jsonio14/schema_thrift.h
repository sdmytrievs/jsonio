#pragma once

#include <memory>
#include <map>
#include "jsonio14/schema.h"

namespace jsonio14 {


/// Definition of one field in thrift structure
/// [id:] [requiredness] type FieldName [=default] [,|;]
class ThriftFieldDef : public FieldDef
{

public:

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

    int f_id;                         ///< Key id
    std::string f_name;               ///< Field Name
    std::vector<FieldType> f_type_id; ///< Type or all "typeId"+"type"+"elemTypeId"+"elemTypeId"  - all levels
    FieldRequered f_required;         ///< A requiredness attribute (REQURED_FIELD)
    std::string  f_default;           ///< Default value
    std::string  inserted_default;    ///< Default value from editor
    std::string  class_name;          ///< "class": Struct or enum name for corresponding field type
    std::string  f_doc;               ///< "doc" - Comment string
    double minval;
    double maxval;
};

/// Thrift structs definition
class ThriftStructDef: public StructDef
{

    void readSchema( const JsonBase* object );
    void read_type_spec( const JsonBase* object,
                         ThriftFieldDef& value, const char* keyspec, const std::string& typeID);

public:

    /// Constructor - read information from json/bson schema
    ThriftStructDef( const JsonBase* object ): StructDef()
    {
        readSchema( object );
    }

};

/// Thrift enums definition
class ThriftEnumDef: public EnumDef
{

    void readEnum( const JsonBase* object );

public:

    /// ThriftEnumDef - read information from json schema
    ThriftEnumDef( const JsonBase* object )
    {
        readEnum( object );
    }
};

/// Thrift schema definition
class ThriftSchema : public SchemasData
{

    void readSchema( const JsonBase* object );
    void setTypeMap();

public:

    static std::map<std::string, int> name_to_thrift_types;

    /// ThriftSchema - constructor
    ThriftSchema():SchemasData()
    {
        setTypeMap();
    }

    /// Read thrift schema from json file fileName
    void addSchemaFile( const std::string& file_path ) override;
};

} // namespace jsonio14


