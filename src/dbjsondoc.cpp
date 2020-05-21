
#include "jsonio14/dbjsondoc.h"
#include "jsonio14/jsondump.h"

namespace jsonio14 {

DBJsonDocument *DBJsonDocument::newJsonDocument( const DataBase &dbconnect, const std::string &coll_name,
                                                 const std::vector<std::string> &key_template_fields)
{
    if( coll_name.empty() )
        return nullptr;
    return  new DBJsonDocument( dbconnect, coll_name, key_template_fields );
}


DBJsonDocument *DBJsonDocument::newJsonDocumentQuery( const DataBase &dbconnect, const std::string &coll_name,
                                                      const std::vector<std::string> &key_template_fields, DBQueryBase &&query)
{
    if( coll_name.empty() )
        return nullptr;
    auto new_document = new DBJsonDocument( dbconnect, coll_name, key_template_fields );
    // init internal selection block
    new_document->setQuery( std::move(query) );
    return new_document;
}


void DBJsonDocument::updateQuery()
{
    if( query_result.get() == nullptr )
        return;

    query_result->clear();
    SetReaded_f setfnc = [&]( const std::string& jsondata )
    {
        auto jsonFree = json::loads( jsondata );
        auto key = collection_from->getKeyFrom( jsonFree );
        query_result->add_line( key,  jsonFree, false );
    };

    collection_from->selectQuery( query_result->condition(), setfnc );
    // Create a thread using member function
    //std::thread th(&TDBCollection::selectQuery, _collection, _queryResult->getQuery().getQueryCondition(), setfnc );
    //th.detach();
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

} // namespace jsonio14
