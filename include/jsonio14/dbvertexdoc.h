#pragma once

#include "jsonio14/dbschemadoc.h"

namespace jsonio14 {

/// Map unique fields values<-> record key
using unique_fields_map_t = std::map<std::vector<std::string>, std::string >;

std::string collectionNameFromSchema( const std::string& schema_name );

/// Definition of graph databases chain
class DBVertexDocument : public DBSchemaDocument
{

    /// Do it before remove document with key from collection.
    void before_remove( const std::string&  ) override;
    /// Do it after remove document with key from collection.
    void after_remove( const std::string&  ) override;
    /// Do it before write document to database
    void before_save_update( std::string&  ) override;
    /// Do it after write document to database
    void after_save_update( const std::string&  ) override;

public:

    /// Generate query for the document-handle.
    static DBQueryBase idQuery( const std::string& id_document )
    {
        return  DBQueryBase( std::string("{ \"_id\" : \"")+ id_document + "\" }", DBQueryBase::qTemplate);
    }

    /// Generate a query for the edges.
    static DBQueryBase edgesQuery( const std::string& id_document, DBQueryBase::QType query_type, const std::string& edge_collections );

    /// Generate all edges query ( edge_collections - comma-separated edges collections list )
    static DBQueryBase allEdgesQuery( const std::string& id_vertex, const std::string& edge_collections = "" )
    {
        return edgesQuery( id_vertex, DBQueryBase::qEdgesAll, edge_collections );
    }

    /// Generate the outgoing edges query ( edge_collections - comma-separated edges collections list ).
    static DBQueryBase outEdgesQuery( const std::string& id_vertex, const std::string& edge_collections = "" )
    {
        return edgesQuery( id_vertex, DBQueryBase::qEdgesFrom, edge_collections );
    }

    /// Generate the incoming edges query ( edge_collections - comma-separated edges collections list ).
    static DBQueryBase inEdgesQuery( const std::string& id_vertex, const std::string& edge_collections = "" )
    {
        return edgesQuery( id_vertex, DBQueryBase::qEdgesTo, edge_collections );
    }

    static DBVertexDocument* newVertexDocumentQuery( const DataBase& dbconnect, const std::string& aschema_name, DBQueryBase&& query );
    static DBVertexDocument* newVertexDocument( const DataBase& dbconnect, const std::string& aschema_name );

    ///  Constructor collection&document
    DBVertexDocument( const std::string& aschema_name, const DataBase& dbconnect ):
        DBVertexDocument( aschema_name,  dbconnect, "vertex", collectionNameFromSchema( aschema_name ) )
    {
        resetSchema( aschema_name, true );
    }

    ///  Constructor document
    DBVertexDocument( const std::string& aschema_name, DBCollection* collection  ):
        DBSchemaDocument( aschema_name, collection ),
        change_schema_mode(false)
    {
        resetSchema( aschema_name, true );
    }

    ///  Destructor
    virtual ~DBVertexDocument(){}

    /// Change the mode of reading documents.
    /// If true, when reading a new record from DB, current schema can be changed.
    void setMode( bool mode )
    {
        change_schema_mode = mode;
    }

    /// Test&change schema
    void testUpdateSchema( const std::string&  pkey ) override;

    /// Change current schema
    void resetSchema( const std::string& aschemaName, bool change_queries ) override;

    /// Load document from json string
    /// \return current document key
    std::string recFromJson( const std::string& jsondata ) override
    {
        test_schema( jsondata );
        setJson( jsondata );
        return getKeyFromCurrent( );
    }

    /// Test existence outgoing edges.
    bool existOutEdges( const std::string& id )
    {
        auto query = outEdgesQuery( id );
        return existDocumentsByQuery( std::move(query) );
    }
    /// Build the document-handles list of the outgoing edges.
    std::vector<std::string> getOutEdgesKeys( const std::string& id )
    {
        auto query = outEdgesQuery( id );
        return getKeysByQuery( std::move(query) );
    }

    /// Test existence incoming edges.
    bool existInEdges( const std::string& id )
    {
        auto query = inEdgesQuery( id );
        return existDocumentsByQuery( std::move(query) );
    }
    /// Build the document-handle list of the incoming edges.
    std::vector<std::string> getInEdgesKeys( const std::string& id )
    {
        auto query = inEdgesQuery( id );
        return getKeysByQuery( std::move(query) );
    }


    /// Define new Vertex document
    void setVertexObject( const std::string& aschemaName, const FieldSetMap& fldvalues );
    /// Change current schema document data
    void updateVertexObject( const std::string& aschemaName, const FieldSetMap& fldvalues );

    /// Creates a new vertex document in the collection from the given fldvalues data.
    /// \param  fldvalues - data to save
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document
    std::string CreateVertex( const std::string& aschemaName, const FieldSetMap& fldvalues, bool testValues = false )
    {
        setVertexObject( aschemaName, fldvalues );
        return CreateWithTestValues( testValues );
    }

    /// Update/create a vertex document.
    /// \param  fldvalues - values to update
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    void UpdateVertex( const std::string& aschemaName, const FieldSetMap& fldvalues, bool testValues = false )
    {
        updateVertexObject( aschemaName, fldvalues );
        UpdateWithTestValues( testValues );
    }

    /// Build map of fields-value pairs
    FieldSetMap loadRecordFields( const std::string& id, const std::vector<std::string>& queryFields );

    // service functions

    /// Extract label by id  ( old  using query - observed )
    std::string extractLabelById( const std::string& id );

    /// Extract label from id  ( no query )
    std::string extractLabelFromId( const std::string& id );

    /// Extract schema by id  ( no query )
    virtual std::string  extractSchemaFromId( const std::string& id  )
    {
        std::string label = extractLabelFromId( id );
        label.pop_back(); // delete last "s"
        return _schema->getVertexName( label );
    }


protected:

    /// When reading new record from DB, schema Name can be changed.
    bool change_schema_mode = false;
    /// This field must contain "vertex"
    std::string object_type;
    /// This label should be the same for all documents
    std::string object_label;

    /// Names of fields group to be unique.
    std::vector<std::string>  unique_fields_names;
    /// Table to save unique fields values
    unique_fields_map_t unique_fields_values;              ///< map to save unique fields values

    /// Type constructor
    DBVertexDocument( const std::string& aschema_name, const DataBase& dbconnect,
                       const std::string& collection_type, const std::string& collection_name ):
        DBSchemaDocument( aschema_name, dbconnect, collection_type, collection_name ),
        change_schema_mode(false)
    {}

    /// Build default query for collection
    /// (could be changed to AQL)
    DBQueryBase make_default_query_template() const override
    {
        return DBQueryBase(std::string("{ \"_label\" : \"")+ object_label + "\" }", DBQueryBase::qTemplate);
    }

    unique_fields_map_t::iterator unique_line_by_id( const std::string& idschem );

    /// Test true type and label for schema
    void test_schema( const std::string& jsondata );

    /// Change base collections
    void update_collection( const std::string& aschemaName );

    /// Init uniqueFields when load collection
    void load_unique_fields();
};

} // namespace jsonio14

