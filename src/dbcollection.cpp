#include "jsonio14/jsonbase.h"
#include "jsonio14/dbvertexdoc.h"
#include "jsonio14/jsonfree.h"
#include "jsonio14/jsondump.h"

namespace jsonio14 {



std::string make_template_key( const JsonBase& object, const std::vector<std::string>&  key_template_fields )
{
    std::string kpart, key_temlpate = "";
    JSONIO_THROW_IF( !object.isTop(), "DBCollection", 20,
                      " illegal function on level 'make_template_key'." );

    for( auto const &keyfld : key_template_fields )
    {
        object.get_value_via_path( keyfld, kpart, std::string() );
        trim(kpart);
        if( !key_temlpate.empty() )
            key_temlpate +=";";   // delimiter for generated key
        key_temlpate += kpart;
    }

    return key_temlpate;
}

//-------------------------------------------------------------
// TDBCollection - This class contains the structure of Data Base Collection
//-------------------------------------------------------------


// Default configuration of the Data Base
DBCollection::DBCollection(  const DataBase& adbconnect,
                             const std::string& name  ):
    coll_name( name ), db_connect(adbconnect),
    documents_list(), key_record_map(),
    keysmap_mutex(), documents_mutex()
{ }


// Open collection file and build linked record list
void DBCollection::load()
{
    loadCollection();
}

// Close file, clear key list
void DBCollection::close()
{
    closeCollection();
}


// Reconnect DataBase (collection)
void DBCollection::reload()
{
    close();
    load();
    std::shared_lock<std::shared_mutex> g(documents_mutex);
    for( auto doc: documents_list )
    {
        doc->updateQuery();            // Rebuild internal table of values
        DBVertexDocument* vertex_doc = dynamic_cast<DBVertexDocument*>(doc);
        if( vertex_doc )
            vertex_doc->load_unique_fields();
    }
}

std::string DBCollection::getKeyFrom( const JsonBase& object )
{
    std::string key_str, kbuf;
    for( const auto& keyfld: keyFields())
    {
        object.get_key_via_path( keyfld, kbuf, std::string("undef") );
        trim(kbuf);
        key_str += kbuf;
    }
    return key_str;
}

std::string DBCollection::generateOid( const std::string& key_template )
{
    if( key_template.empty())
        return "";
    auto sanitized_handle = db_driver()->sanitization(key_template);
    return coll_name+"/"+key_from_template(sanitized_handle);
}


bool DBCollection::existsDocument( const std::string &key ) const
{
    //std::shared_lock<std::shared_mutex> g(keysmap_mutex);
    //std::lock_guard<std::shared_mutex> g(keysmap_mutex);
    std::shared_lock<std::shared_mutex> g(keysmap_mutex);
    return  key_record_map.find(key)  != key_record_map.end();
}

std::string DBCollection::createDocument( JsonBase& data_object )
{
    auto new_key = getKeyFrom( data_object );

    JSONIO_THROW_IF( !is_allowed( new_key ), "DBCollection", 11,
                     " some of characters cannot be used inside _key values '" + new_key +"'." );

    JSONIO_THROW_IF( existsDocument( new_key ), "DBCollection", 12,
                     " two records with the same key '" + new_key +"'." );

    {
        std::unique_lock<std::shared_mutex> g(keysmap_mutex);
        std::unique_ptr<char> second = nullptr;
        // save record to data base
        std::string ret_id = db_driver()->create_record( name(), second, data_object );
        JSONIO_THROW_IF( ret_id.empty(), "DBCollection", 13," error saving record '" + new_key +"'." );
        data_object.set_oid( ret_id );
        new_key = getKeyFrom( data_object );
        key_record_map[new_key] = std::move(second);
    }
    return new_key;
}

std::string DBCollection::createDocument( DBDocumentBase *document )
{
    auto new_key = createDocument( document->current_data() );
    document->add_line( new_key, document->current_data(), false );
    return new_key;
}

bool DBCollection::readDocument( JsonBase& data_object, const std::string &key )
{
    std::shared_lock<std::shared_mutex> g(keysmap_mutex);
    auto itr = key_record_map.find(key);
    JSONIO_THROW_IF( itr==key_record_map.end(), "DBCollection", 14,
                      " record to retrive does not exist '" + key +"'." );
    return db_driver()->read_record( name(), itr, data_object);
}


void DBCollection::readDocument(DBDocumentBase *document, const std::string &key)
{
    if( !readDocument( document->current_data(), key ) )
        JSONIO_THROW( "DBCollection", 15, " error loading record '" + key +"'." );
}

std::string DBCollection::updateDocument( const JsonBase& data_object )
{
    std::string rec_id = "";
    auto key = getKeyFrom( data_object );

    {
        std::shared_lock<std::shared_mutex> g(keysmap_mutex);
        auto itr = key_record_map.find(key);
        JSONIO_THROW_IF( itr==key_record_map.end(), "DBCollection", 16,
                         " record to update does not exist '" + key +"'." );

        rec_id = db_driver()->update_record( name(), itr, data_object );
    }

    std::shared_lock<std::shared_mutex> g(documents_mutex);
    for( auto itdoc:  documents_list)
        itdoc->update_line( rec_id, data_object );
    return rec_id;
}

std::string DBCollection::saveDocument( JsonBase& data_object, const std::string &key)
{
    if( existsDocument(key) )
    {
        data_object.set_oid( key );
        return createDocument( data_object );
    }
    else
    {
        return updateDocument( data_object );
    }
}

std::string DBCollection::saveDocument( DBDocumentBase *document, const std::string &key)
{
    auto rec_id = saveDocument( document->current_data(), key );
    JSONIO_THROW_IF( rec_id.empty(), "DBCollection", 17,
                      " error saving record '" + key +"'." );
    document->add_line( rec_id, document->current_data(), true );
    return rec_id;
}


bool DBCollection::deleteDocument( const std::string &key )
{
    bool rec_deleted = false;
    {
        std::unique_lock<std::shared_mutex> g(keysmap_mutex);
        auto itr = key_record_map.find(key);
        JSONIO_THROW_IF( itr==key_record_map.end(), "DBCollection", 18,
                         " record to delete does not exist '" + key +"'." );

        rec_deleted = db_driver()->delete_record( name(), itr);
        if( rec_deleted)
            key_record_map.erase(itr);
    }
    if( rec_deleted )
    {
        std::shared_lock<std::shared_mutex> g(documents_mutex);
        for( auto itdoc:  documents_list)
            itdoc->delete_line( key );
    }
    return rec_deleted;
}


void DBCollection::deleteDocument(DBDocumentBase *document)
{
    auto key = document->getKeyFromCurrent();
    if( !deleteDocument( key ) )
        JSONIO_THROW( "DBCollection", 19, " deleting of record '" + key +"'." );
    document->delete_line( key );
}

//-----------------------------------------------------------------

//  Other thread
void DBCollection::loadCollectionFile(  const std::set<std::string>& query_fields )
{
    std::unique_lock<std::shared_mutex> g(keysmap_mutex);
    std::cout << "loadCollectionFile locked" << std::endl;
    SetReadedKey_f setfnc = [=]( const std::string& jsondata, const std::string& keydata )
    {
        //std::cout << jsondata << std::endl;
        add_record_to_map( jsondata, keydata );
    };
    db_driver()->all_query( name(), query_fields, setfnc );
    std::cout << "loadCollectionFile unlocked" << std::endl;
}

void DBCollection::loadCollection()
{
    clear_keysmap();
    // create collection if no exist
    db_driver()->create_collection( name(), coll_type );
    //loadCollectionFile( keyFields() );
    // Create a thread using member function
    std::thread th( &DBCollection::loadCollectionFile, this,  keyFields() );
    th.detach();
    std::this_thread::sleep_for(std::chrono::microseconds(1)); // wait start thread
}


// Gen new oid from key template
std::string DBCollection::key_from_template( const std::string& key_template ) const
{
    std::string id_head = name() + "/" + key_template;

    auto ids_list = get_ids_as_template( id_head  );
    if( ids_list.empty() )
        return key_template;

    int ii=0;
    while( ids_list.find( id_head + "_" + std::to_string(ii) ) != ids_list.end() )
        ii++;
    return key_template + "_" + std::to_string(ii);
}


void DBCollection::add_record_to_map( const std::string& jsondata, const std::string& keydata )
{
    std::unique_ptr<char> second = nullptr;
    db_driver()->set_server_key(second, keydata);

    auto jsFree = json::loads( jsondata );
    auto id_key = getKeyFrom( jsFree );

    auto it_new =  key_record_map.insert( std::pair<std::string,std::unique_ptr<char> >(
                                              id_key, std::move(second) ) );
    // Test unique keys name before add the record(s)
    if( it_new.second == false)
        JSONIO_THROW( "DBCollection", 20, " two records with the same key '" + id_key +"'." );
}

std::vector<std::string> DBCollection::ids_from_keys(const std::vector<std::string> &rkeys) const
{
    std::vector<std::string> ids;
    std::shared_lock<std::shared_mutex> g(keysmap_mutex);

    for( auto rkey: rkeys )
    {
        const auto itrl = key_record_map.find( rkey );
        if( itrl != key_record_map.end() )
            ids.push_back( db_driver()->get_server_key( itrl->second ) );
    }
    return ids;
}


std::set<std::string> DBCollection::get_ids_as_template( const std::string& id_head ) const
{
    std::set<std::string> ids_list;
    std::shared_lock<std::shared_mutex> g(keysmap_mutex);
    for( const auto& it: key_record_map )
    {
        if( it.second.get() != nullptr )
            if( std::string(it.second.get()).substr(0, id_head.length()) == id_head  )
                ids_list.insert( it.second.get() );
    }
    return ids_list;
}

bool DBCollection::is_allowed( const std::string &akey ) const
{
    return akey == db_driver()->sanitization(akey);
}


} // namespace jsonio14

