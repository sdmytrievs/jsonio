#include <cstring>
#include <iostream>

#include "jsonio14/dbdriverarango.h"
#include "jsonio14/dbqueryarango.h"
#include "jsonio14/io_settings.h"

#include "jsonarango/arangocollection.h"
#include "jsonarango/arangocollection.h"

namespace jsonio14 {


// Default Constructor
ArangoDBClient::ArangoDBClient()
{
    arangocpp::ArangoDBConnection connectData;
    /// ???? connectData.getFromSettings(ioSettings().defaultArangoDB());
    reset_db_connection( connectData );
}

void ArangoDBClient::reset_db_connection( const arangocpp::ArangoDBConnection& aconnectData )
{
    arando_connect = std::make_shared<arangocpp::ArangoDBConnection>(aconnectData);
    arando_db = std::make_shared<arangocpp::ArangoDBCollectionAPI>(aconnectData);
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


// Run query
//  \param collname - collection name
//  \param query -  query  condition (where)
//  \param setfnc - function for set up readed data
void ArangoDBClient::select_query( const std::string& collname, const DBQueryBase& query,  SetReaded_f setfnc )
{
///????    if( query.getType() == DBQueryData::qEdgesFrom ||
//            query.getType() == DBQueryData::qEdgesTo  ||
//            query.getType() == DBQueryData::qEdgesAll    )
//        pdata->SearchEdgesToFrom( collname,  query, setfnc );
//    else
//        pdata->SelectQuery( collname,  query, setfnc );
}


void ArangoDBClient::all_query( const std::string& collname, const std::set<std::string>& queryFields,
                                SetReadedKey_f setfnc )
{
 ///????   arando_db->AllQueryFields( isComlexFields(queryFields), collname, queryFields, setfnc );
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

// list collection names
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
