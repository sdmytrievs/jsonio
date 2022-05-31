#include <cstring>
#include "jsonio17/dbdriverarango.h"
#include "jsonio17/dbquerybase.h"
#include "jsonio17/io_settings.h"
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"

namespace jsonio17 {

// Get settings data from ison section
arangocpp::ArangoDBConnection getFromSettings( const SectionSettings& section, bool rootdata )
{
    arangocpp::ArangoDBConnection connect_data;

    if( rootdata )
    {
        connect_data.serverUrl = section.value<std::string>( "DB_URL", arangocpp::ArangoDBConnection::local_server_endpoint );
        connect_data.databaseName = section.value<std::string>( "DBRootName", arangocpp::ArangoDBConnection::local_root_database );
        connect_data.user.name = section.value<std::string>( "DBRootUser", arangocpp::ArangoDBConnection::local_root_username );
        connect_data.user.password = section.value<std::string>( "DBRootPassword", arangocpp::ArangoDBConnection::local_root_password );
        connect_data.user.access = "rw";
    }
    else
    {
        connect_data.serverUrl = section.value<std::string>( "DB_URL", arangocpp::ArangoDBConnection::local_server_endpoint );
        connect_data.databaseName = section.value<std::string>( "DBName", arangocpp::ArangoDBConnection::local_server_database );
        connect_data.user.name = section.value<std::string>( "DBUser", arangocpp::ArangoDBConnection::local_server_username );
        connect_data.user.password = section.value<std::string>( "DBUserPassword", arangocpp::ArangoDBConnection::local_server_password );
        connect_data.user.access = section.value<std::string>( "DBAccess", "rw" );
    }
    return connect_data;
}


// Default Constructor
ArangoDBClient::ArangoDBClient():AbstractDBDriver()
{
    arangocpp::ArangoDBConnection aconnect_data =
            arangocpp::connectFromSettings( ioSettings().dump(), false );
    reset_db_connection( aconnect_data );
}

const arangocpp::ArangoDBConnection &ArangoDBClient::connect_data() const
{
    return *arando_connect.get();
}

void ArangoDBClient::reset_db_connection( const arangocpp::ArangoDBConnection& aconnect_data )
{
    try {
        arando_connect = std::make_shared<arangocpp::ArangoDBConnection>(aconnect_data);
        arando_db = std::make_shared<arangocpp::ArangoDBCollectionAPI>(aconnect_data);
        io_logger->debug("ArangoDBClient::reset_db_connection url: {}", arando_connect->fullHost() );
    }
    catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

void ArangoDBClient::create_collection(const std::string& collname, const std::string& ctype)
{
    try {
        arando_db->createCollection( collname, ctype );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

// list collection names
std::set<std::string> ArangoDBClient::get_collections_names( CollTypes ctype )
{
    try {
        return arando_db->collectionNames( to_arrango_types( ctype ) );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

void ArangoDBClient::set_server_key(std::string &second, const std::string& id_key )
{
    second = id_key;
}

std::string ArangoDBClient::create_record(const std::string &collname,
                                          std::string& second, const JsonBase& recdata)
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 2,
                          " try to create document into read only mode." );

        auto  jsonrec = recdata.dump();
        auto new_id =  arando_db->createDocument( collname, jsonrec);
        set_server_key( second, new_id );
        return new_id;

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

// Retrive one record from the collection
bool ArangoDBClient::read_record( const std::string& collname, keysmap_t::iterator&it, JsonBase& recdata )
{
    try {
        std::string jsonrec;
        std::string rid = get_server_key( it->second );
        auto ret =  arando_db->readDocument( collname, rid, jsonrec );
        recdata.loads(jsonrec);
        return ret;

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

// Removes record from the collection
bool ArangoDBClient::delete_record( const std::string& collname, keysmap_t::iterator& itr  )
{
    try  {
        std::string rid = get_server_key( itr->second );
        return arando_db->deleteDocument( collname, rid );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

// Save/update record in the collection
std::string ArangoDBClient::update_record( const std::string& collname, keysmap_t::iterator& it, const JsonBase& recdata )
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 3,
                          " try to create/update document into read only mode." );

        auto  jsonrec = recdata.dump();
        std::string rid = get_server_key( it->second );
        rid = arando_db->updateDocument( collname, rid, jsonrec );
        return rid;

    }  catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}


void ArangoDBClient::select_query( const std::string& collname, const DBQueryBase& query,  SetReaded_f setfnc )
{
    try {
        auto arango_query = query.arando_query;
        arando_db->selectQuery( collname,  *arango_query, setfnc );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}


void ArangoDBClient::all_query( const std::string& collname, const std::set<std::string>& query_fields,
                                SetReadedKey_f setfnc )
{
    try {
        fields2query_t  arango_query_fields;
        if( !is_comlex_fields(query_fields) )
        {
            // select only query_fields
            for ( const auto& it : query_fields)
                arango_query_fields[it] = it;
        }
        arando_db->selectAll( collname, arango_query_fields, setfnc );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}


void ArangoDBClient::delete_edges(const std::string& collname, const std::string& vertexid )
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 4,
                          " try to remove edge into read only mode." );
        arando_db->removeEdges( collname, vertexid );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

void ArangoDBClient::fpath_collect( const std::string& collname, const std::string& fpath,
                                    std::vector<std::string>& values )
{
    try {
        arando_db->collectQuery( collname, fpath, values);

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

void ArangoDBClient::lookup_by_ids( const std::string& collname,  const std::vector<std::string>& ids,
                                    SetReaded_f setfnc )
{
    try {
        // we can use ids and keys
        arando_db->lookupByKeys( collname, ids, setfnc);

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

void ArangoDBClient::remove_by_ids( const std::string& collname,  const std::vector<std::string>& ids )
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClient", 5,
                          " to remove documents into read only mode." );
        arando_db->removeByKeys( collname, ids );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

std::string ArangoDBClient::sanitization(const std::string &documentHandle)
{
    try {

        return  arando_db->sanitization(documentHandle);

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClient", e.id, e.what() );
    }
}

//-----------------------------------------------------------------------------------------

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
    for( const auto& fld: queryFields )
        if( fld.find(".") !=  std::string::npos )
            return true;
    return false;
}


} // namespace jsonio17
