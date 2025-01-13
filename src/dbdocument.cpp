#include "jsonio/dbdocument.h"
#include "jsonio/dbconnect.h"
#include "jsonio/jsonfree.h"
#include "jsonio/jsondump.h"

namespace jsonio {


// Default configuration of the Data Base
DBDocumentBase::DBDocumentBase( const DataBase& dbconnect, const std::string& collection_type, const std::string& collection_name  ):
    collection_from(nullptr), query_result(nullptr), query_result_mutex()
{
    collection_from = dbconnect.collection( collection_name, collection_type  );
    collection_from->addDocument(this);
}


// Default configuration of the Data Base
DBDocumentBase::DBDocumentBase( DBCollection* collection  ):
    collection_from( collection ), query_result(nullptr), query_result_mutex()
{
    collection_from->addDocument(this);
}


void DBDocumentBase::setQuery( const DBQueryDef& querydef )
{
    {
        std::lock_guard<std::shared_mutex> g(query_result_mutex);
        if(query_result.get() == nullptr )
            query_result.reset( new DBQueryResult( querydef ) );
        else
            query_result->setQuery( querydef );
    }
    updateQuery();
}

void DBDocumentBase::updateQuery()
{
    {
        std::shared_lock<std::shared_mutex> g(query_result_mutex);
        if( query_result.get() == nullptr )
            return;
    }
    io_logger->debug("Document {} update query: {}", collection_from->name(), query_result->condition().queryString());

    //update_query();
    // Create a thread using member function
    std::thread th( &DBDocumentBase::update_query, this );
    th.detach();
    std::this_thread::sleep_for(std::chrono::microseconds(1)); // wait start thread
}


// Set&execute query for document
void DBDocumentBase::setQuery( const DBQueryBase& query, std::vector<std::string> fields_list)
{
    std::shared_ptr<DBQueryBase> internal_query;
    if( query.empty()  )
        internal_query = std::make_shared<DBQueryBase>(make_default_query_template());
    else
        internal_query = std::make_shared<DBQueryBase>(query);

    if( fields_list.empty()  )
    {
        fields_list = make_default_query_fields();
    }
    setQuery( DBQueryDef( internal_query, fields_list )  );
}


std::string DBDocumentBase::createWithTestValues( bool test_values )
{
    auto key = getKeyFromCurrent();

    if( key.empty() && test_values )
    {
        key = getKeyFromValue(current_data());
        if( !key.empty() )
        {
            updateDocument( key );
            return key;
        }
    }
    return createDocument( key );
}

void DBDocumentBase::updateWithTestValues( bool test_values )
{
    auto key = getKeyFromCurrent();

    if( !existsDocument(key) )
       createWithTestValues( test_values );
    else
       updateDocument( key );
}

field_value_map_t DBDocumentBase::extract_fields(const std::vector<std::string> queryFields,
                                                 const JsonBase& domobj) const
{
    std::string valbuf;
    field_value_map_t res;
    for( auto& ent: queryFields )
    {
        domobj.get_value_via_path( ent, valbuf, std::string("") );
        res[ent] = valbuf;
    }
    return res;
}

field_value_map_t DBDocumentBase::extract_fields(const std::vector<std::string> queryFields,
                                                 const std::string &jsondata) const
{
    auto jsonFree = json::loads( jsondata );
    return extract_fields( queryFields, jsonFree );
}

values_table_t DBDocumentBase::downloadDocuments(const DBQueryBase &query,
                                                 const std::vector<std::string> &queryFields) const
{
    values_table_t records_values;

    SetReaded_f setfnc = [&, queryFields]( const std::string& jsondata )
    {
        auto flds_values = DBDocumentBase::extract_fields( queryFields, jsondata );
        values_t row_data;
        for( const auto& fld: queryFields)
            row_data.push_back( flds_values[fld] );
        records_values.push_back(row_data);
    };

    collection_from->selectQuery( query, setfnc );
    return records_values;
}

values_table_t DBDocumentBase::downloadDocuments(const std::vector<std::string> &keys,
                                                 const std::vector<std::string> &queryFields) const
{
    values_table_t records_values;

    SetReaded_f setfnc = [&, queryFields]( const std::string& jsondata )
    {
        auto flds_values = DBDocumentBase::extract_fields( queryFields, jsondata );
        values_t row_data;
        for( const auto& fld: queryFields)
            row_data.push_back( flds_values[fld] );
        records_values.push_back(row_data);
    };

    collection_from->lookupByKeys( keys, setfnc );
    return records_values;
}

values_t DBDocumentBase::selectQuery(const DBQueryBase &query) const
{
    values_t result_data;

    SetReaded_f setfnc = [&result_data]( const std::string& jsondata )
    {
        result_data.push_back(jsondata);
    };

    collection_from->selectQuery( query, setfnc );
    return result_data;
}

values_t DBDocumentBase::lookupByKeys(const std::vector<std::string> &rkeys) const
{
    values_t result_data;

    SetReaded_f setfnc = [&result_data]( const std::string& jsondata )
    {
        result_data.push_back(jsondata);
    };

    collection_from->lookupByKeys( rkeys, setfnc );
    return result_data;
}

bool DBDocumentBase::existDocumentsByQuery( DBQueryBase &&query ) const
{
    fields2query_t query_fields;
    query_fields["_id"] = "_id";
    query.setQueryFields( query_fields );
    values_t result_data = selectQuery( query );
    return !result_data.empty();
}

std::vector<std::string> DBDocumentBase::getKeysByQuery(DBQueryBase &&query) const
{
    fields2query_t query_fields;
    query_fields["_id"] = "_id";
    query.setQueryFields( query_fields );
    values_t result_data = selectQuery( query );

    std::vector<std::string> keys;
    for( const auto& data: result_data )
    {
        auto akey = extract_string_json("_id", data);
        //regexp_replace( data, ".*\"_id\"\\s*:\\s*\"([^\"]*)\".*", "$1");
        io_logger->trace("{}: {}", akey, data);
        keys.push_back(akey);
    }
    return keys;
}


} // namespace jsonio
