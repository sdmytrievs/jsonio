
#include "jsonio17/dbjsondoc.h"
#include "jsonio17/jsondump.h"

namespace jsonio17 {

DBJsonDocument *DBJsonDocument::newJsonDocument( const DataBase& dbconnect, const std::string& coll_name,
                                                 const std::vector<std::string>& key_template_fields)
{
    if( coll_name.empty() )
        return nullptr;
    return  new DBJsonDocument( dbconnect, coll_name, key_template_fields );
}


DBJsonDocument *DBJsonDocument::newJsonDocumentQuery( const DataBase& dbconnect, const std::string& coll_name,
                                                      const std::vector<std::string>& key_template_fields,
                                                      const DBQueryBase& query)
{
    if( coll_name.empty() )
        return nullptr;
    auto new_document = new DBJsonDocument( dbconnect, coll_name, key_template_fields );
    // init internal selection block
    new_document->setQuery( query );
    return new_document;
}


void DBJsonDocument::update_query()
{
    std::unique_lock<std::shared_mutex> g(query_result_mutex);

    query_result->clear();
    SetReaded_f setfnc = [&]( const std::string& jsondata )
    {
        auto jsonFree = json::loads( jsondata );
        auto key = collection_from->getKeyFrom( jsonFree );
        query_result->add_line( key,  jsonFree, false );
    };

    collection_from->selectQuery( query_result->condition(), setfnc );
}


void DBJsonDocument::before_save_update( std::string &key )
{
    if( key.empty() )
    {
        std::string key_template = current_object["_key"].toString();
        auto new_id = genOid( key_template );
        setOid( new_id );
        key = new_id;
    }
}

} // namespace jsonio17
