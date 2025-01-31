#include <cstring>
#include "jsonio/dbdriverar_old.h"
#include "jsonio/dbquerybase.h"
#include "jsonio/io_settings.h"
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"

namespace jsonio {

// Get settings data from ison section
//arangocpp::ArangoDBConnection getFromSettings( const SectionSettings& section, bool rootdata )
//{
//    arangocpp::ArangoDBConnection connect_data;

//    if( rootdata )
//    {
//        connect_data.serverUrl = section.value<std::string>( "DB_URL", arangocpp::ArangoDBConnection::local_server_endpoint );
//        connect_data.databaseName = section.value<std::string>( "DBRootName", arangocpp::ArangoDBConnection::local_root_database );
//        connect_data.user.name = section.value<std::string>( "DBRootUser", arangocpp::ArangoDBConnection::local_root_username );
//        connect_data.user.password = section.value<std::string>( "DBRootPassword", arangocpp::ArangoDBConnection::local_root_password );
//        connect_data.user.access = "rw";
//    }
//    else
//    {
//        connect_data.serverUrl = section.value<std::string>( "DB_URL", arangocpp::ArangoDBConnection::local_server_endpoint );
//        connect_data.databaseName = section.value<std::string>( "DBName", arangocpp::ArangoDBConnection::local_server_database );
//        connect_data.user.name = section.value<std::string>( "DBUser", arangocpp::ArangoDBConnection::local_server_username );
//        connect_data.user.password = section.value<std::string>( "DBUserPassword", arangocpp::ArangoDBConnection::local_server_password );
//        connect_data.user.access = section.value<std::string>( "DBAccess", "rw" );
//    }
//    return connect_data;
//}


// Default Constructor
ArangoDBClientOld::ArangoDBClientOld():AbstractDBDriver()
{
    arangocpp::ArangoDBConnection aconnect_data =
            arangocpp::connectFromSettings( ioSettings().dump(), false );
    reset_db_connection( aconnect_data );
}

ArangoDBClientOld::ArangoDBClientOld(const std::string &db_url, const std::string &db_user,
                               const std::string &user_passwd, const std::string &db_name): AbstractDBDriver()
{
    arangocpp::ArangoDBConnection aconnect_data(db_url, db_user, user_passwd, db_name);
    reset_db_connection(aconnect_data);
}

const arangocpp::ArangoDBConnection &ArangoDBClientOld::connect_data() const
{
    return *arando_connect.get();
}

AbstractDBDriver *ArangoDBClientOld::clone(const std::string &new_db_name)
{
    auto connection_data = connect_data();
    connection_data.databaseName = new_db_name;
    return new ArangoDBClientOld{connection_data};
}

void ArangoDBClientOld::reset_db_connection( const arangocpp::ArangoDBConnection& aconnect_data )
{
    try {
        arando_connect = std::make_shared<arangocpp::ArangoDBConnection>(aconnect_data);
        arando_db = std::make_shared<arangocpp::ArangoDBCollectionAPI>(aconnect_data);
        io_logger->debug("ArangoDBClientOld::reset_db_connection url: {}", arando_connect->fullHost() );
    }
    catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

void ArangoDBClientOld::create_collection(const std::string& collname, const std::string& ctype)
{
    try {
        arando_db->createCollection( collname, ctype );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

// list collection names
std::set<std::string> ArangoDBClientOld::get_collections_names( CollTypes ctype )
{
    try {
        return arando_db->collectionNames( to_arrango_types( ctype ) );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

void ArangoDBClientOld::set_server_key(std::string &second, const std::string& id_key )
{
    second = id_key;
}

std::string ArangoDBClientOld::create_record(const std::string &collname,
                                          std::string& second, const JsonBase& recdata)
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClientOld", 2,
                          " try to create document into read only mode." );

        auto  jsonrec = recdata.dump();
        auto new_id =  arando_db->createDocument( collname, jsonrec);
        set_server_key( second, new_id );
        return new_id;

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

// Retrive one record from the collection
bool ArangoDBClientOld::read_record( const std::string& collname, keysmap_t::iterator&it, JsonBase& recdata )
{
    try {
        std::string jsonrec;
        std::string rid = get_server_key( it->second );
        auto ret =  arando_db->readDocument( collname, rid, jsonrec );
        recdata.loads(jsonrec);
        return ret;

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

// Removes record from the collection
bool ArangoDBClientOld::delete_record( const std::string& collname, keysmap_t::iterator& itr  )
{
    try  {
        std::string rid = get_server_key( itr->second );
        return arando_db->deleteDocument( collname, rid );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

// Save/update record in the collection
std::string ArangoDBClientOld::update_record( const std::string& collname, keysmap_t::iterator& it, const JsonBase& recdata )
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClientOld", 3,
                          " try to create/update document into read only mode." );

        auto  jsonrec = recdata.dump();
        std::string rid = get_server_key( it->second );
        rid = arando_db->updateDocument( collname, rid, jsonrec );
        return rid;

    }  catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}


void ArangoDBClientOld::select_query( const std::string& collname, const DBQueryBase& query,  SetReaded_f setfnc )
{
    try {
        auto arango_query = query.arando_query;
        arando_db->selectQuery( collname,  *arango_query, setfnc );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}


void ArangoDBClientOld::all_query( const std::string& collname, const std::set<std::string>& query_fields,
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
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}


void ArangoDBClientOld::delete_edges(const std::string& collname, const std::string& vertexid )
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClientOld", 4,
                          " try to remove edge into read only mode." );
        arando_db->removeEdges( collname, vertexid );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

void ArangoDBClientOld::fpath_collect( const std::string& collname, const std::string& fpath,
                                    std::vector<std::string>& values )
{
    try {
        arando_db->collectQuery( collname, fpath, values);

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

void ArangoDBClientOld::lookup_by_ids( const std::string& collname,  const std::vector<std::string>& ids,
                                    SetReaded_f setfnc )
{
    try {
        // we can use ids and keys
        arando_db->lookupByKeys( collname, ids, setfnc);

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

void ArangoDBClientOld::remove_by_ids( const std::string& collname,  const std::vector<std::string>& ids )
{
    try {
        JSONIO_THROW_IF( arando_connect->readonlyDBAccess(), "ArangoDBClientOld", 5,
                          " to remove documents into read only mode." );
        arando_db->removeByKeys( collname, ids );

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

std::string ArangoDBClientOld::sanitization(const std::string &documentHandle)
{
    try {

        return  arando_db->sanitization(documentHandle);

    } catch(arangocpp::arango_exception& e)
    {
        JSONIO_THROW( "ArangoDBClientOld", e.id, e.what() );
    }
}

//-----------------------------------------------------------------------------------------

arangocpp::CollectionTypes ArangoDBClientOld::to_arrango_types( CollTypes ctype ) const
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
bool ArangoDBClientOld::is_comlex_fields( const std::set<std::string>& queryFields)
{
    for( const auto& fld: queryFields )
        if( fld.find(".") !=  std::string::npos )
            return true;
    return false;
}

//void create_ArangoDB_local_database_if_no_exist(const std::string &db_url,  const std::string &db_user,
//                                       const std::string &user_passwd, const std::string &db_name)
//{
//    if(db_url.find("localhost")) {
//        try {
//            arangocpp::ArangoDBConnection local_root{db_url, db_user, user_passwd, "_system"};
//            std::shared_ptr<arangocpp::ArangoDBRootClient> root_client(new arangocpp::ArangoDBRootClient{local_root});
//            if(!root_client->existDatabase(db_name)) {
//                root_client->createDatabase(db_name);
//            }
//        }
//        catch(std::exception& e)
//        {
//            io_logger->error("Exception when creating local database {}, if they do not exist {}", db_name, e.what());
//        }
//    }
//}

} // namespace jsonio
