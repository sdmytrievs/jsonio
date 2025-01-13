#pragma once

#include "jsonio/dbdocument.h"
#include "jsonio/jsonschema.h"


namespace jsonio {

/// \class DBSchemaDocument - implementation of the database document as schema-based JSON.
class DBSchemaDocument : public  DBDocumentBase
{
    friend class GraphTraversal;

protected:

    /// Do it before write document to database
    void before_save_update( std::string& key ) override;

public:

    static DBSchemaDocument *newSchemaDocumentQuery( const DataBase& dbconnect, const std::string& aschema_name,
                                                     const std::string& collection_name,
                                                     const DBQueryBase& query = DBQueryBase::emptyQuery(),
                                                     const std::vector<std::string>& fields_list = {}  );

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

    /// Link to internal data
    const JsonSchema& loaded_data() const override
    {
        return  current_schema_object;
    }

    /// Use jsonpath to modify any value in a JSON object.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// @return true if jsonpath present in a JSON object.
    bool loadValueViaPath( const std::string& jsonpath, const std::string& val  )
    {
        current_data().load_value_via_path( jsonpath, val );
        return true;
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

    /// Build table of fields values by query ( add default values from chema )
    ///  \param query - query condition
    ///  \param  queryFields - list of fields (columns) in result table
    ///  \return table of field values
    values_table_t downloadDocumentsbySchema( const DBQueryBase&  query, const std::vector<std::string>& queryFields ) const;

    /// Build table of fields values by their keys ( add default values from chema )
    ///  \param rkeys - list of top level record keys
    ///  \param  queryFields - list of fields (columns) in result table
    ///  \return table of field values
    values_table_t downloadDocumentsbySchema( const std::vector<std::string>& keys, const std::vector<std::string>& queryFields ) const;


    using DBDocumentBase::setQuery;
    /// Set&execute query for document
    void setQuery( const DBQueryDef& querydef ) override
    {
       DBDocumentBase::setQuery( querydef );
       std::shared_lock<std::shared_mutex> g(query_result_mutex);
       query_result->updateSchema( schema_name );
    }

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

    /// Run current query, rebuild internal table of values
    void update_query() override;

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

} // namespace jsonio

