
#include "jsonio14/dbquerybase.h"
#include "jsonio14/jsonfree.h"

namespace jsonio14 {

void DBQueryBase::setBindVars(const JsonBase &bindObject)
{
   setBindVars(bindObject.dump());
}

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
    object.get_value_via_path(  "name", key_name, std::string("undefined") );
    object.get_value_via_path(  "comment", rec_comment, std::string("") );
    object.get_value_via_path(  "qschema", toschema, std::string("") );

    auto condq = (object.field("condition"));
    if( condq != nullptr )
      query_condition->fromJson( *condq );

    object.get_value_via_path(  "collect", fields_collect, {} );
}


//---------------------------------------------------------------

// Make line to view table
void DBQueryResult::node_to_values( const JsonBase* node, values_t& values ) const
{
//   values.clear();
//   string kbuf;
//   JsonDom *data;

//   for(uint ii=0; ii< query.getFieldsCollect().size(); ii++ )
//   {
//       data = node->field(  query.getFieldsCollect()[ii] );
//       if( data == nullptr || !data->getValue( kbuf ) )
//            kbuf = "---";
//       strip( kbuf );
//       values.push_back(kbuf);
//   }
}


// Delete line from view table
void DBQueryResult::addLine(const std::string &key_str, const JsonBase *nodedata, bool isupdate)
{
//    vector<string> values;
//    nodeToValues( nodedata, values );

//    if( isupdate)
//    { auto it =  queryValues.find(keyStr);
//      if( it != queryValues.end() )
//      {   it->second = values;
//          return;
//      }
//    }

//    queryValues.insert(pair<string,vector<string>>(keyStr, values ));

}

void DBQueryResult::updateLine(const std::string &key_str, const JsonBase *nodedata)
{
//    vector<string> values;
//    nodeToValues( nodedata, values );

//    auto it =  queryValues.find(keyStr);
//      if( it != queryValues.end() )
//         it->second = values;
}

void DBQueryResult::deleteLine( const std::string& keyStr )
{
//  auto it =  queryValues.find(keyStr);
//    if( it != queryValues.end() )
//        queryValues.erase(it);
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

std::size_t DBQueryResult::getKeysValues(std::vector<std::string> &aKeyList, std::vector<values_t> &aValList, const char *keypart, MatchKeyTemplate_f compareTemplate) const
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

std::size_t DBQueryResult::getKeysValues(std::vector<std::string> &aKeyList, std::vector<values_t> &aValList,
                                         const std::vector<std::string> &fieldnames, const std::vector<std::string> &fieldvalues) const
{
    aKeyList.clear();
    aValList.clear();

//    uint ii;
//    std::vector<uint> fieldindexes;

//    for( auto fname: fieldnames )
//    {
//        for( ii=0; ii< query.getFieldsCollect().size(); ii++ )
//        {
//            if( fname == query.getFieldsCollect()[ii])
//            {
//                fieldindexes.push_back(ii);
//                break;
//            }
//        }
//        if( ii == query.getFieldsCollect().size() )
//            return 0;  // no field
//    }

//    auto it = queryValues.begin();
//    while( it != queryValues.end() )
//    {
//        bool getdata=true;

//        for( ii=0; ii<fieldindexes.size(); ii++ )
//        {
//            if( it->second[fieldindexes[ii]] != fieldvalues[ii] )
//            {
//                getdata = false;
//                break;
//            }
//        }

//        if( getdata )
//        {
//            aKeyList.push_back( it->first );
//            aValList.push_back( it->second );
//        }
//        it++;
//    }

    return aKeyList.size();

}

std::string DBQueryResult::getKeyFromValue(const JsonBase *node) const
{
//    uint ii;
//    vector<string> value;
//    nodeToValues( node, value );
//    auto it = queryValues.begin();
//    while( it != queryValues.end() )
//    {
//        for( ii=0; ii< query.getFieldsCollect().size(); ii++ )
//        {
//           if( it->second[ii] != value[ii] &&
//               query.getFieldsCollect()[ii] != "_id" &&
//               query.getFieldsCollect()[ii] != "_key" &&
//               query.getFieldsCollect()[ii] != "_rev" )
//             break;
//        }
//        if( ii >= query.getFieldsCollect().size() )
//          return it->first;
//        //if( it->second == value )
//        //  return it->first;
//       it++;
//    }
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


