#include "jsonio17/dbschemadoc.h"
#include "jsonio17/jsondump.h"
#include "jsonio17/io_settings.h"

namespace jsonio17 {

DBSchemaDocument *DBSchemaDocument::newSchemaDocumentQuery( const DataBase& dbconnect, const std::string& aschema_name,
                                                            const std::string& collection_name, const DBQueryBase& query)
{
    if( aschema_name.empty() || collection_name.empty() )
        return nullptr;
    auto new_document =  new DBSchemaDocument( aschema_name, dbconnect, collection_name );
    // init internal selection block
    new_document->setQuery( query );
    return new_document;
}

DBSchemaDocument *DBSchemaDocument::newSchemaDocument( const DataBase& dbconnect, const std::string& aschema_name,
                                                       const std::string& collection_name)
{
    if( aschema_name.empty() || collection_name.empty() )
        return nullptr;
    return  new DBSchemaDocument( aschema_name, dbconnect, collection_name );
}


void DBSchemaDocument::resetSchema( const std::string &aschema_name, bool change_queries )
{
    // We use the same collection for different schemas?
    if( schema_name != aschema_name  )
    {
        schema_name = aschema_name;
        current_schema_object = JsonSchema::object(schema_name);
    }
    std::unique_lock<std::shared_mutex> g(query_result_mutex);
    if( change_queries && query_result.get() != nullptr  )
    {
      // for new schema need new query
       query_result.reset();
    }
}

void DBSchemaDocument::before_save_update(std::string &key)
{
    if( key.empty() )
    {
        std::string key_template = current_schema_object["_key"].toString();
        auto new_id = genOid( key_template );
        setOid( new_id );
        key = new_id;
    }
}

std::string DBSchemaDocument::genOid( const std::string &key_template )
{
    auto thetemplate = key_template;
    if( thetemplate.empty() )
    {
        auto schema_struct = ioSettings().Schema().getStruct( schema_name );
        if( schema_struct != nullptr )
        {
           thetemplate = make_template_key( current_schema_object,
                                            schema_struct->getKeyTemplateList() );
        }
    }
    return collection_from->generateOid( thetemplate );
}

values_table_t DBSchemaDocument::downloadDocumentsbySchema(const DBQueryBase &query, const std::vector<std::string> &queryFields) const
{
    values_table_t records_values;

    SetReaded_f setfnc = [&, queryFields]( const std::string& jsondata )
    {
        auto jsonbase = json::loads(schema_name, jsondata);
        auto flds_values = extract_fields( queryFields, jsonbase );
        values_t row_data;
        for( const auto& fld: queryFields)
            row_data.push_back( flds_values[fld] );
        records_values.push_back(row_data);
    };

    collection_from->selectQuery( query, setfnc );
    return records_values;
}

values_table_t DBSchemaDocument::downloadDocumentsbySchema( const std::vector<std::string>& keys, const std::vector<std::string>& queryFields ) const
{
    values_table_t records_values;

    SetReaded_f setfnc = [&, queryFields]( const std::string& jsondata )
    {
        auto jsonbase = json::loads(schema_name, jsondata);
        auto flds_values = extract_fields( queryFields, jsonbase );
        values_t row_data;
        for( const auto& fld: queryFields)
            row_data.push_back( flds_values[fld] );
        records_values.push_back(row_data);
    };

    collection_from->lookupByKeys( keys, setfnc );
    return records_values;
}


void DBSchemaDocument::update_query()
{
    std::unique_lock<std::shared_mutex> g(query_result_mutex);

    query_result->clear();
    SetReaded_f setfnc = [&]( const std::string& jsondata )
    {
        if( query_result->condition().isOnlyFieldsQuery() )
        {
            auto json_free = json::loads( jsondata );
            auto key = collection_from->getKeyFrom( json_free/*, query_result->condition().queryFields()*/ );
            query_result->add_line_fields( key, json_free, query_result->condition().queryFields() );
        }
        else
        {
           auto json_schema = json::loads( schema_name, jsondata );
           auto key = collection_from->getKeyFrom( json_schema );
           query_result->add_line( key,  json_schema, false );
        }
    };

    collection_from->selectQuery( query_result->condition(), setfnc );
}


std::vector<std::string> DBSchemaDocument::make_default_query_fields() const
{
    const StructDef* schema_struct  = ioSettings().Schema().getStruct( schema_name );
    if( schema_struct == nullptr )
        return DBDocumentBase::make_default_query_fields();

    std::vector<std::string> key_fields, ids_or_names = schema_struct->getSelectedList();
    if( ids_or_names.empty() )
    {
        auto field_it = schema_struct->cbegin();
        while( field_it != schema_struct->cend() )
        {
            if( field_it->get()->required() == FieldDef::fld_required )
                key_fields.push_back(field_it->get()->name());
            field_it++;
        }
    }
    else  // generate names if index
    {
       key_fields.clear();
       for( const auto& id_or_key: ids_or_names )
       {
           auto pos = id_or_key.find_first_not_of("0123456789.");
           if( pos == std::string::npos  )
           {
               auto ids = split_int( id_or_key, ".");
               auto the_name = schema_struct->getPathFromIds( ids );
               key_fields.push_back( the_name );
           }
           else
           {
               key_fields.push_back( id_or_key );
           }
       }
    }
    auto key_flds = collection_from->keyFields();
    key_fields.insert(key_fields.end(), key_flds.begin(), key_flds.end() );
    return key_fields;
}

field_value_map_t DBSchemaDocument::extract_fields(const std::vector<std::string> queryFields, const std::string &jsondata) const
{
    auto jsonbase = json::loads(schema_name, jsondata);
    //auto jsonbase = json::loads( jsondata );   free json
    return extract_fields( queryFields, jsonbase );
}


} // namespace jsonio17
