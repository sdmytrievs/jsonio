#pragma once

#include "jsonio14/dbdocument.h"
#include "jsonio14/jsonschema.h"


namespace jsonio14 {

/// \class DBSchemaDocument - implementation of the database document as schema-based JSON.
class DBSchemaDocument : public  DBDocumentBase
{
    friend class GraphTraversal;

protected:

    /// Do it before write document to database
    void before_save_update( std::string& key ) override;

public:

    static DBSchemaDocument *newSchemaDocumentQuery( const DataBase& dbconnect, const std::string& aschema_name,
                         const std::string& collection_name,  DBQueryBase&& query  );

    static DBSchemaDocument* newSchemaDocument( const DataBase& dbconnect,
              const std::string& aschema_name, const std::string& collection_name  );

    ///  Constructor
    DBSchemaDocument( const std::string& aschema_name,
                      const DataBase& dbconnect, const std::string& collection_name ):
        DBSchemaDocument( aschema_name, dbconnect, "schema", collection_name )
    {}

    ///  Constructor
    DBSchemaDocument( const std::string& aschema_name, DBCollection* collection  ):
        DBDocumentBase( collection ),
        schema_name(aschema_name), current_schema_object(JsonSchema::object(schema_name))
    {}

    ///  Destructor
    virtual ~DBSchemaDocument(){}

    /// Change current schema
    virtual void resetSchema( const std::string& aschema_name, bool change_queries );

    /// Get the name of thrift schema
    const std::string& getSchemaName() const
    {
        return schema_name;
    }

    /// Generate new document-handle (_id) or other pointer of location
    std::string genOid( const std::string& key_template ) override;

    /// Return curent document as json string
    std::string getJson( bool dense = false ) const override
    {
        return current_schema_object.dump(dense);
    }
    /// Load document from json string
    void setJson( const std::string& json_document ) override
    {
        current_schema_object.loads(json_document);
    }

    using DBDocumentBase::setQuery;
    /// Set&execute query for document
    void setQuery( const DBQueryDef& querydef ) override
    {
       DBDocumentBase::setQuery( querydef );
       query_result->updateSchema( schema_name );
    }

    /// Run current query, rebuild internal table of values
    void updateQuery() override;

protected:

    /// Current schema name
    std::string schema_name = "";

    /// Current document object
    JsonSchema current_schema_object;

    /// Constructor
    DBSchemaDocument( const std::string& aschema_name, const DataBase& dbconnect,
                      const std::string& collection_type, const std::string& collection_name ):
        DBDocumentBase( dbconnect, collection_type, collection_name ),
        schema_name(aschema_name), current_schema_object(JsonSchema::object(schema_name))
    {}

    /// Link to internal data
    JsonSchema& current_data() const override
    {
        return  const_cast<JsonSchema&>( current_schema_object );
    }

    /// Build default query for collection ( by default all documents )
    DBQueryBase make_default_query_template() const override
    {
        return DBQueryBase(DBQueryBase::qAll);
    }

    /// Build default query fields ( by default internal )
    std::vector<std::string>  make_default_query_fields() const override;

    using DBDocumentBase::extract_fields;
    field_value_map_t extract_fields( const std::vector<std::string> queryFields, const std::string& jsondata ) const override;
};

} // namespace jsonio14

