#include <cstring>
#include <iostream>

#include "jsonio14/dbdriverarango.h"
#include "jsonio14/dbquerybase.h"
#include "jsonio14/io_settings.h"

#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoconnect.h"

namespace jsonio14 {


// Default Constructor
ArangoDBClient::ArangoDBClient():AbstractDBDriver()
{
    arangocpp::ArangoDBConnection aconnect_data;
    /// ???? aconnect_data.getFromSettings(ioSettings().defaultArangoDB());
    reset_db_connection( aconnect_data );
}

void ArangoDBClient::reset_db_connection( const arangocpp::ArangoDBConnection& aconnect_data )
{
    arando_connect = std::make_shared<arangocpp::ArangoDBConnection>(aconnect_data);
    arando_db = std::make_shared<arangocpp::ArangoDBCollectionAPI>(aconnect_data);
}

// Create collection if no exist
void ArangoDBClient::create_collection(const std::string& collname, const std::string& ctype)
{
    arando_db->createCollection( collname, ctype );
}

// list collection names
std::set<std::string> ArangoDBClient::get_collections_names( CollTypes ctype )
{
    return arando_db->collectionNames( to_arrango_types( ctype ) );
}

// Retrive one record from the collection
bool ArangoDBClient::load_record( const std::string& collname, keysmap_t::iterator&it, JsonBase* recdata )
{
    std::string jsonrec;
    std::string rid = get_server_key( it->second.get() );
    auto ret =  arando_db->readDocument( collname, rid, jsonrec );
    recdata->loads(jsonrec);
    return ret;
}

// Removes record from the collection
bool ArangoDBClient::remove_record( const std::string& collname, keysmap_t::iterator& itr  )
{
    JARANGO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 1, " try to remove document into read only mode." );

    std::string rid = get_server_key( itr->second.get() );
    return arando_db->deleteDocument( collname, rid );
}

// Save/update record in the collection
std::string ArangoDBClient::save_record( const std::string& collname, keysmap_t::iterator& it, const JsonBase* recdata )
{
    JARANGO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 2, " try to create/update document into read only mode." );

    auto  jsonrec = recdata->dump();
    std::string newrid;
    if( it->second.get() == nullptr ) // new record
    {
        newrid = arando_db->createDocument( collname, jsonrec);
        if( !newrid.empty() )
            set_server_key( newrid, it );
    }
    else
    {
        std::string rid = get_server_key( it->second.get() );
        newrid = arando_db->updateDocument( collname, rid, jsonrec );
    }

    return (newrid);
}


void ArangoDBClient::select_query( const std::string& collname, const DBQueryBase& query,  SetReaded_f setfnc )
{
    auto arango_query = query.arando_query;
    arando_db->selectQuery( collname,  *arango_query, setfnc );
}


void ArangoDBClient::all_query( const std::string& collname, const std::set<std::string>& query_fields,
                                SetReadedKey_f setfnc )
{
    fields2query_t  arango_query_fields;
    if( !is_comlex_fields(query_fields) )
    {
        // select only query_fields
        for ( const auto& it : query_fields)
            arango_query_fields[it] = it;
    }
    arando_db->selectAll( collname, arango_query_fields, setfnc );
}


void ArangoDBClient::delete_edges(const std::string& collname, const std::string& vertexid )
{
    JARANGO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 3, " try to remove edge into read only mode." );
    arando_db->removeEdges( collname, vertexid );
}

void ArangoDBClient::fpath_collect( const std::string& collname, const std::string& fpath,
                                    std::vector<std::string>& values )
{
    arando_db->collectQuery( collname, fpath, values);
}

void ArangoDBClient::lookup_by_ids( const std::string& collname,  const std::vector<std::string>& ids,
                                   SetReaded_f setfnc )
{
    // we can use ids and keys
    arando_db->lookupByKeys( collname, ids, setfnc);
}

void ArangoDBClient::remove_by_ids( const std::string& collname,  const std::vector<std::string>& ids )
{
    JARANGO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 4, " to remove documents into read only mode." );
    arando_db->removeByKeys( collname, ids );
}

//-----------------------------------------------------------------------------------------

void jsonio14::ArangoDBClient::set_server_key(const std::string& key, keysmap_t::iterator& itr)
{
    char *bytes = new char[key.length()+1];
    strncpy( bytes, key.c_str(), key.length() );
    bytes[key.length()] = '\0';
    itr->second = std::unique_ptr<char>(bytes);
}

arangocpp::CollectionTypes ArangoDBClient::to_arrango_types( CollTypes ctype ) const
{
    arangocpp::CollectionTypes arango_type = arangocpp::CollectionTypes::Vertex;
    switch( ctype )
    {
    case AbstractDBDriver::clVertex:
        arango_type = arangocpp::CollectionTypes::Vertex;
        break;
    case AbstractDBDriver::clEdge:
        arango_type = arangocpp::CollectionTypes::Edge;
        break;
    case AbstractDBDriver::clAll:
        arango_type = arangocpp::CollectionTypes::All;
        break;
    }
    return arango_type;
}

// Test if we have complex field path
bool ArangoDBClient::is_comlex_fields( const std::set<std::string>& queryFields)
{
    for( auto fld: queryFields )
        if( fld.find(".") !=  std::string::npos )
            return true;
    return false;
}


} // namespace jsonio14
