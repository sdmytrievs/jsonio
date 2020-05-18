
#include "jsonio14/dbquerybase.h"
#include "jsonio14/jsonfree.h"
#include "jsonarango/arangoquery.h"

namespace jsonio14 {

arangocpp::ArangoDBQuery::QueryType to_arrango_query_type( DBQueryBase::QType qtype )
{
    arangocpp::ArangoDBQuery::QueryType arango_type = arangocpp::ArangoDBQuery::Undef;
    switch( qtype )
    {
    case DBQueryBase::qUndef:
        arango_type = arangocpp::ArangoDBQuery::Undef;
        break;
    case DBQueryBase::qTemplate:
        arango_type = arangocpp::ArangoDBQuery::Template;
        break;
    case DBQueryBase::qAll:
        arango_type = arangocpp::ArangoDBQuery::All;
        break;
    case DBQueryBase::qEdgesFrom:
        arango_type = arangocpp::ArangoDBQuery::EdgesFrom;
        break;
    case DBQueryBase::qEdgesTo:
        arango_type = arangocpp::ArangoDBQuery::EdgesTo;
        break;
    case DBQueryBase::qEdgesAll:
        arango_type = arangocpp::ArangoDBQuery::EdgesAll;
        break;
    case DBQueryBase::qAQL:
        arango_type = arangocpp::ArangoDBQuery::AQL;
        break;
    case DBQueryBase::qEJDB:
        break;
    }
    return arango_type;
}

DBQueryBase::QType to_base_query_types( arangocpp::ArangoDBQuery::QueryType arango_type )
{
    DBQueryBase::QType qtype = DBQueryBase::qUndef;
    switch( arango_type )
    {
    case arangocpp::ArangoDBQuery::Undef:
        qtype = DBQueryBase::qUndef;
        break;
    case arangocpp::ArangoDBQuery::Template:
        qtype = DBQueryBase::qTemplate;
        break;
    case arangocpp::ArangoDBQuery::All:
        qtype = DBQueryBase::qAll;
        break;
    case arangocpp::ArangoDBQuery::EdgesFrom:
        qtype = DBQueryBase::qEdgesFrom;
        break;
    case arangocpp::ArangoDBQuery::EdgesTo:
        qtype = DBQueryBase::qEdgesTo;
        break;
    case arangocpp::ArangoDBQuery::EdgesAll:
        qtype = DBQueryBase::qEdgesAll;
        break;
    case arangocpp::ArangoDBQuery::AQL:
        qtype = DBQueryBase::qAQL;
        break;
    }
    return qtype;
}


DBQueryBase::DBQueryBase(DBQueryBase::QType atype)
{
    arando_query = std::make_shared<arangocpp::ArangoDBQuery>( to_arrango_query_type(atype) );
}

DBQueryBase::DBQueryBase(const std::string &condition, QType atype)
{
    arando_query = std::make_shared<arangocpp::ArangoDBQuery>( condition, to_arrango_query_type(atype) );
}

void DBQueryBase::toJson(JsonBase &object) const
{
    object.clear();
    object.set_value_via_path( "style", static_cast<int>(type()) );
    object.set_value_via_path( "find", queryString() );
    object.set_value_via_path( "bind", bindVars() );
    object.set_value_via_path( "options", options() );
    object.set_value_via_path( "fields", queryFields() ); // map
}

void DBQueryBase::fromJson(const JsonBase &object)
{
    int atype;
    std::string data;
    fields2query_t fields;

    object.get_value_via_path(  "style", atype, -1 );
    object.get_value_via_path(  "find", data, std::string("") );
    arando_query = std::make_shared<arangocpp::ArangoDBQuery>(
                data, static_cast<arangocpp::ArangoDBQuery::QueryType>(atype) );
    object.get_value_via_path(  "bind", data, std::string("") );
    arando_query->setBindVars(data);
    object.get_value_via_path(  "options", data, std::string("") );
    arando_query->setOptions(data);
    object.get_value_via_path(  "fields", fields, {} );
    arando_query->setQueryFields(fields);
}

bool DBQueryBase::empty() const
{
    return arando_query->empty();
}

DBQueryBase::QType DBQueryBase::type() const
{
    return to_base_query_types( arando_query->type() );
}

const std::string &DBQueryBase::queryString() const
{
    return arando_query->queryString();
}

void DBQueryBase::setBindVars(const std::string &jsonBind )
{
    arando_query->setBindVars(jsonBind);
}

void DBQueryBase::setBindVars(const JsonBase &bind_object )
{
    setBindVars(bind_object.dump());
}

const std::string &DBQueryBase::bindVars() const
{
    return arando_query->bindVars();
}

void DBQueryBase::setOptions(const std::string &json_options )
{
    arando_query->setOptions(json_options);
}

const std::string &DBQueryBase::options() const
{
    return arando_query->options();
}

void DBQueryBase::setQueryFields(const fields2query_t &map_fields)
{
    arando_query->setQueryFields(map_fields);
}

const fields2query_t &DBQueryBase::queryFields() const
{
    return arando_query->queryFields();
}

//----------------------------------------------------------------------------------------------

void DBQueryDef::toJson(JsonFree& object) const
{
    object.clear();
    object.set_value_via_path( "name" ,  key_name);
    object.set_value_via_path( "comment", rec_comment );
    object.set_value_via_path( "qschema", toschema );

    decltype(object)& condq = object.add_object_via_path("condition");
    query_condition->toJson( condq );
    object.set_value_via_path("collect", fields_collect);
}

void DBQueryDef::fromJson(const JsonBase& object)
{
    object.get_value_via_path( "name", key_name, std::string("undefined") );
    object.get_value_via_path( "comment", rec_comment, std::string("") );
    object.get_value_via_path( "qschema", toschema, std::string("") );

    auto condq = (object.field("condition"));
    if( condq != nullptr )
        query_condition->fromJson( *condq );

    object.get_value_via_path( "collect", fields_collect, {} );
}

//---------------------------------------------------------------

// Make line to view table
void DBQueryResult::node_to_values( const JsonBase* node, values_t& values ) const
{
    values.clear();
    std::string kbuf;
    for( const auto& afield: query_data.fields() )
    {
        node->get_value_via_path( afield, kbuf, std::string("---") );
        trim( kbuf );
        values.push_back(kbuf);
    }
}

void DBQueryResult::add_line( const std::string &key_str, const JsonBase *nodedata, bool isupdate )
{
    values_t values;
    node_to_values( nodedata, values );

    if( isupdate )
    {
        auto it =  query_result.find(key_str);
        if( it != query_result.end() )
        {   it->second = values;
            return;
        }
    }
    query_result.insert(std::pair<std::string,values_t>( key_str, values ));
}

void DBQueryResult::update_line(const std::string &key_str, const JsonBase *nodedata)
{
    values_t values;
    node_to_values( nodedata, values );

    auto it =  query_result.find(key_str);
    if( it != query_result.end() )
        it->second = values;
}

void DBQueryResult::delete_line( const std::string& key_str )
{
    auto it =  query_result.find(key_str);
    if( it != query_result.end() )
        query_result.erase(it);
}

std::size_t DBQueryResult::getKeysValues(std::vector<std::string> &aKeyList, std::vector<values_t> &aValList) const
{
    aKeyList.clear();
    aValList.clear();

    for( const auto& it: query_result )
    {
        aKeyList.push_back( it.first );
        aValList.push_back( it.second );
    }

    return aKeyList.size();
}

std::size_t DBQueryResult::getKeysValues(std::vector<std::string> &aKeyList, std::vector<values_t> &aValList,
                                         const char *keypart, MatchKeyTemplate_f compareTemplate) const
{
    aKeyList.clear();
    aValList.clear();

    for( const auto& it: query_result )
    {
        auto key = it.first;
        if( compareTemplate(keypart, key ) )
        {
          aKeyList.push_back( key );
          aValList.push_back( it.second );
        }
    }

    return aKeyList.size();
}

std::size_t DBQueryResult::getKeysValues( std::vector<std::string> &aKeyList, std::vector<values_t> &aValList,
                                         const std::vector<std::string> &fieldnames, const std::vector<std::string> &fieldvalues) const
{
    aKeyList.clear();
    aValList.clear();

    std::size_t ii;
    std::vector<std::size_t> fieldindexes;

    for( auto fname: fieldnames )
    {
        for( ii=0; ii< query_data.fields().size(); ii++ )
        {
            if( fname == query_data.fields()[ii])
            {
                fieldindexes.push_back(ii);
                break;
            }
        }
        if( ii == query_data.fields().size() )
            return 0;  // no field
    }

    for( const auto& it: query_result )
    {
        bool getdata=true;
        for( ii=0; ii<fieldindexes.size(); ii++ )
        {
            if( it.second[fieldindexes[ii]] != fieldvalues[ii] )
            {
                getdata = false;
                break;
            }
        }
        if( getdata )
        {
            aKeyList.push_back( it.first );
            aValList.push_back( it.second );
        }
    }
    return aKeyList.size();
}

std::string DBQueryResult::getKeyFromValue( const JsonBase* node ) const
{
    std::size_t ii;
    values_t values;
    node_to_values( node, values );

    for( const auto& it: query_result )
    {
        for( ii=0; ii< query_data.fields().size(); ++ii )
        {
           if( it.second[ii] != values[ii] &&
               query_data.fields()[ii] != "_id" &&
               query_data.fields()[ii] != "_key" &&
               query_data.fields()[ii] != "_rev" )
             break;
        }
        if( ii >= query_data.fields().size() )
          return it.first;
    }
    return "";
}

std::string DBQueryResult::getFirstKey() const
{
    auto it = query_result.begin();
    if( it != query_result.end() )
       return it->first;
    return "";  // empty table
}


} // namespace jsonio14


