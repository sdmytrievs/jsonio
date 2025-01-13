#pragma once

#include "jsonio/dbvertexdoc.h"

namespace jsonio {


/// Definition of graph databases chain: Handling Edges
/// \class DBEdgeDocument - implementation of the database edge as schema-based JSON.
/// A Graph consists of vertices and edges. Edges are stored as documents in edge collections.
class DBEdgeDocument : public DBVertexDocument
{
    void before_remove( const std::string&  ) override {}

public:

    static DBEdgeDocument* newEdgeDocumentQuery( const DataBase& dbconnect, const std::string& aschema_name,
                                                 const DBQueryBase& query = DBQueryBase::emptyQuery()  );

    static DBEdgeDocument* newEdgeDocument( const DataBase& dbconnect, const std::string& aschema_name );

    ///  Constructor collection&document
    DBEdgeDocument( const std::string& aschema_name, const DataBase& dbconnect ):
        DBEdgeDocument( aschema_name,  dbconnect, "edge", collectionNameFromSchema( aschema_name ) )
    {
        resetSchema( aschema_name, true );
    }
    ///  Constructor document
    DBEdgeDocument( const std::string& aschema_name, DBCollection* collection  ):
        DBVertexDocument( aschema_name, collection )
    {
        resetSchema( aschema_name, true );
    }

    ///  Destructor
    virtual ~DBEdgeDocument(){}


    /// Define new edge document
    void setEdgeObject( const std::string& aschema_name, const std::string& outV,
                        const std::string& inV, const field_value_map_t& fldvalues );

    /// Creates a new edge document in the collection from the given fldvalues data.
    /// \param  fldvalues - data to save
    /// \param  test_values - If test_values is true, we compare the current data with the internally loaded values,
    /// and if all the values are the same, then we update the selected record instead of creating new ones.
    /// \return new key of document
    std::string createEdge( const std::string& aschema_name, const std::string& outV,
                            const std::string& inV, const field_value_map_t& fldvalues, bool test_values = false )
    {
        setEdgeObject( aschema_name, outV, inV, fldvalues );
        return createWithTestValues( test_values );
    }

    /// Extract schema by id  ( no query )
    std::string  extractSchemaFromId( const std::string& oid  ) const override;

protected:

    /// Type constructor
    DBEdgeDocument( const std::string& aschema_name, const DataBase& dbconnect,
                    const std::string& collection_type, const std::string& collection_name ):
        DBVertexDocument( aschema_name, dbconnect, collection_type, collection_name )
    { }

    /// Build default query for collection
    /// (could be changed to AQL)
    DBQueryBase make_default_query_template() const override;


    /// Make Follow Outgoing Edges query - observed
    DBQueryBase outEdgesQueryOld( const std::string& id ) const
    {
        return DBQueryBase( std::string("{\"_type\": \"edge\", \"_from\": \"")+ id + "\" }", DBQueryBase::qEdgesFrom);
    }
    /// Make Follow Outgoing Edges query - observed
    DBQueryBase outEdgesQueryOld( const std::string& edgeLabel, const std::string& idVertex ) const
    {
        std::string queryJson = "{'_type': 'edge', '_label': '"
                + edgeLabel  + "', '_from': '"
                + idVertex + "' }";
        return DBQueryBase( queryJson, DBQueryBase::qEdgesFrom);
    }

    /// Make Follow Incoming Edges query - observed
    DBQueryBase inEdgesQueryOld( const std::string& id ) const
    {
        return DBQueryBase( std::string("{\"_type\": \"edge\", \"_to\": \"")+ id + "\" }", DBQueryBase::qEdgesTo);
    }
    /// Make Follow Incoming Edges query - observed
    DBQueryBase inEdgesQueryOld( const std::string& edgeLabel, const std::string& idVertex ) const
    {
        std::string queryJson = "{'_type': 'edge', '_label': '"
                + edgeLabel  + "', '_to': '"
                + idVertex + "' }";
        return DBQueryBase( queryJson, DBQueryBase::qEdgesTo);
    }

    DBQueryBase make_default_query_old() const;
};

DBEdgeDocument* documentAllEdges( const DataBase& dbconnect );

} // namespace jsonio


