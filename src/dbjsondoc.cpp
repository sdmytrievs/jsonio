
#include "jsonio/dbjsondoc.h"
#include "jsonio/jsondump.h"

namespace jsonio {

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
    try {
        std::lock_guard<std::shared_mutex> g(query_result_mutex);

        query_result->clear();
        SetReaded_f setfnc = [&]( const std::string& jsondata )
        {
            auto jsonFree = json::loads( jsondata );
            auto key = collection_from->getKeyFrom( jsonFree );
            query_result->add_line( key,  jsonFree, false );
        };

        collection_from->selectQuery( query_result->condition(), setfnc );
    }
    catch(jsonio::jsonio_exception& e)
    {
        io_logger->error("Update query jsonio_exception: {}", e.what());
    }
    catch(std::exception& e)
    {
        io_logger->error("Update query  std::exception: {}", e.what());
    }
    catch(...)
    {
        io_logger->error("Undefined update query  exception");
    }
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

} // namespace jsonio
