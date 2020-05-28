#pragma once

#include "jsonio14/dbschemadoc.h"

namespace jsonio14 {

/// Map unique fields values<-> record key
using unique_fields_map_t = std::map<std::vector<std::string>, std::string >;

std::string collectionNameFromSchema( const std::string& aschema_name );

/// Definition of graph databases chain: Handling Vertices
/// \class DBVertexDocument - implementation of the database vertex as schema-based JSON.
/// A Graph consists of vertices and edges. Edges are stored as documents in edge collections.
/// A vertex can be a document of a document collection or of an edge collection (so edges can be used as vertices).
class DBVertexDocument : public DBSchemaDocument
{
    /// Test&change schema
    void before_load( const std::string&  ) override;
    /// Do it before remove document with key from collection.
    /// Remove all edges connected to vertex.
    void before_remove( const std::string&  ) override;
    /// Do it after remove document with key from collection.
    /// Delete from unique map.
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
        change_schema_mode(false), object_type("vertex"), object_label()
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

    /// Change current schema
    void resetSchema( const std::string& aschema_name, bool change_queries ) override;

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
    void setVertexObject( const std::string& aschema_name, const field_value_map_t& fldvalues );
    /// Change current schema document data
    void updateVertexObject( const std::string& aschema_name, const field_value_map_t& fldvalues );

    /// Creates a new vertex document in the collection from the given fldvalues data.
    /// \param  fldvalues - data to save
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document
    std::string createVertex( const std::string& aschema_name, const field_value_map_t& fldvalues, bool testValues = false )
    {
        setVertexObject( aschema_name, fldvalues );
        return createWithTestValues( testValues );
    }

    /// Update/create a vertex document.
    /// \param  fldvalues - values to update
    /// \param  testValues - If testValues is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    void updateVertex( const std::string& aschema_name, const field_value_map_t& fldvalues, bool testValues = false )
    {
        updateVertexObject( aschema_name, fldvalues );
        updateWithTestValues( testValues );
    }

    /// Build map of fields-value pairs
    field_value_map_t loadRecordFields( const std::string& key, const std::vector<std::string>& query_fields )
    {
        readDocument( key );
        return extract_fields( query_fields, current_schema_object );
    }

    /// Extract schema from the document-handle ( no query ).
    virtual std::string  extractSchemaFromId( const std::string& id  );

    /// Init uniqueFields when load collection
    void load_unique_fields();

protected:

    /// When reading new record from DB, schema Name can be changed.
    bool change_schema_mode = false;
    /// This field must contain "vertex"
    std::string object_type;
    /// This label should be the same for all documents
    std::string object_label;

    /// Names of fields group to be unique.
    std::vector<std::string>  unique_fields_names ={};
    /// Table to save unique fields values
    unique_fields_map_t unique_fields_values ={};

    /// Type constructor
    DBVertexDocument( const std::string& aschema_name, const DataBase& dbconnect,
                       const std::string& collection_type, const std::string& collection_name ):
        DBSchemaDocument( aschema_name, dbconnect, collection_type, collection_name ),
        change_schema_mode(false), object_type("vertex"), object_label()
    {}

    /// Build default query for collection
    /// (could be changed to AQL)
    DBQueryBase make_default_query_template() const override
    {
        return DBQueryBase(std::string("{ \"_label\" : \"")+ object_label + "\" }", DBQueryBase::qTemplate);
    }

    /// Test true type and label for schema
    void test_schema( const std::string& jsondata );

    /// Change base collections
    void update_collection( const std::string& aschemaName );

    unique_fields_map_t::iterator unique_line_by_id( const std::string& idschem );

};

} // namespace jsonio14

