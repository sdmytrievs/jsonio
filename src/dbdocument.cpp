#include "jsonio14/dbdocument.h"
#include "jsonio14/dbconnect.h"
#include "jsonio14/jsonfree.h"
#include "jsonio14/jsondump.h"

namespace jsonio14 {


// Default configuration of the Data Base
DBDocumentBase::DBDocumentBase( const DataBase& dbconnect, const std::string& coltype, const std::string& colname  ):
    collection_from(nullptr), query_result(nullptr)
{
    collection_from = dbconnect.getCollection( coltype, colname  );
    collection_from->addDocument(this);
}


// Default configuration of the Data Base
DBDocumentBase::DBDocumentBase( DBCollection* collection  ):
    collection_from( collection ), query_result(nullptr)
{
    collection_from->addDocument(this);
}


void DBDocumentBase::setQuery( const DBQueryDef& querydef )
{
    if(query_result.get() == nullptr )
        query_result.reset( new DBQueryResult( querydef ) );
    else
        query_result->setQuery( querydef );
    updateQuery();
}


// Set&execute query for document
void DBDocumentBase::setQuery( DBQueryBase &&query, std::vector<std::string> fields_list)
{
    if( query.empty()  )
        query = make_default_query_template();

    if( fields_list.empty()  )
        fields_list = make_default_query_fields();

    setQuery( DBQueryDef(  std::make_shared<DBQueryBase>(query), fields_list )  );
}


std::string DBDocumentBase::createWithTestValues( bool testValues )
{
    auto key = getKeyFromCurrent();

    if( key.empty() && testValues )
    {
        key = get_key_from_query_result();
        if( !key.empty() )
        {
            updateDocument( key );
            return key;
        }
    }
    return createDocument( key );
}

void DBDocumentBase::updateWithTestValues( bool testValues )
{
    auto key = getKeyFromCurrent();

    if( !existsDocument(key) )
       createWithTestValues( testValues );
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
        auto flds_values = extract_fields( queryFields, jsondata );
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
        auto flds_values = extract_fields( queryFields, jsondata );
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

    //--std::regex re("\\{\\s*\"_id\"\\s*:\\s*\"([^\"]*)\"\\s*\\}");
    std::string replacement = "$1";
    std::vector<std::string> keys;
    for( const auto& data: result_data )
    {
        auto akey = regexp_replace( data, ".*\"_id\"\\s*:\\s*\"([^\"]*)\".*", replacement);
        //cout << result_data[ii] << "      " << akey << endl;
        keys.push_back(akey);
    }
    return keys;
}


// extern functions ---------------------------------------

//// Extract the string value by key from query
//int extractIntField( const string& key, const string& jsondata )
//{
//    string token;
//    string query2 =  replace_all( jsondata, "\'", "\"");
//    string regstr =  string(".*\"")+key+"\"\\s*:\\s*([+-]?[1-9]\\d*|0).*";
//    std::regex re( regstr );
//    std::smatch match;

//    if( std::regex_search( query2, match, re ))
//    {
//        if (match.ready())
//            token = match[1];
//    }
//    //cout << key << "  token " << token  << endl;
//    if( token.empty() )  // fix bug for null or noexist fields
//        return 0;
//    return stoi(token);
//}

//// Extract the string value by key from query
//string extractStringField( const string& key, const string& jsondata )
//{
//    string token = "";
//    string query2 =  replace_all( jsondata, "\'", "\"");
//    string regstr =  string(".*\"")+key+"\"\\s*:\\s*\"([^\"]*)\".*";
//    std::regex re( regstr );
//    std::smatch match;

//    if( std::regex_search( query2, match, re ))
//    {
//        if (match.ready())
//            token = match[1];
//    }
//    //cout << key << "  token " << token  << endl;
//    return token;
//}

//// Into ArangoDB Query by example ( must be { "a.b" : value } , no { "a": { "b" : value } }
//// https://docs.arangodb.com/3.3/Manual/DataModeling/Documents/DocumentMethods.html
//void addFieldsToQueryAQL( DBQueryData& query, const FieldSetMap& fldvalues )
//{
//    cout << query.getQueryString() << endl;
//    //jsonioErrIf( query.getType() != DBQueryData::qTemplate, "addFieldsToQuery", "Query not template" );

//    auto newQueryString = query.getQueryString();
//    auto type = query.getType();

//    switch(  type )
//    {
//    case DBQueryData::qTemplate:
//    {
//        auto pos = newQueryString.find_last_of("}");
//        newQueryString = newQueryString.substr(0, pos);
//        newQueryString += ", " + query.generateFILTER(fldvalues, true ) +  "}";
//    }
//        break;
//    case DBQueryData::qAll:
//    {
//        type = DBQueryData::qTemplate;
//        newQueryString =  newQueryString += "{ " + query.generateFILTER(fldvalues, true ) +  " }";
//    }
//        break;
//    case DBQueryData::qAQL:
//    {
//        auto pos = newQueryString.find("RETURN");  // add before return
//        string retstring = "";
//        if( pos != string::npos )
//            retstring = newQueryString.substr( pos);
//        newQueryString = newQueryString.substr(0, pos);
//        newQueryString += query.generateFILTER(fldvalues, false, "u" ); // !!! only u
//        newQueryString += retstring;
//    }
//        break;
//    default: jsonioErr( "addFieldsToQueryAQL", "Illegal query type to modify" );

//    }
//    DBQueryData newQuery(newQueryString, type);
//    newQuery.setBindVars( query.getBindVars());
//    newQuery.setOptions( query.options());
//    newQuery.setQueryFields( query.getQueryFields() );
//    query = newQuery;
//    cout << " Result: " <<  query.getQueryString() << endl;
//}

//void addFieldsToQuery( DBQueryData& query, const FieldSetMap& fldvalues )
//{
//    cout << query.getQueryString() << endl;

//    jsonioErrIf( query.getType() != DBQueryData::qTemplate, "addFieldsToQuery", "Query not template" );

//    auto jsonquery = unpackJson(query.getQueryString());
//    for(auto& ent : fldvalues)
//        jsonquery->updateFieldPath( ent.first, ent.second );

//    string newQuery;
//    printNodeToJson( newQuery, jsonquery.get() );
//    query = DBQueryData(newQuery, query.getType());
//    cout << " Result: " <<  query.getQueryString() << endl;
//}


} // namespace jsonio14
