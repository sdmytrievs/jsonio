/// Test example for ArangoDBCollectionAPI the API for manipulating collections and documents into.

#include <iostream>
#include "jsonio/jsonfree.h"
#include "jsonio/dbcollection.h"
#include "jsonio/dbquerybase.h"
#include "jsonio/io_settings.h"
using namespace jsonio;

void printData( const std::string&  title, const std::vector<std::string>& values );

void printData( const std::string&  title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  std::endl;
    std::cout <<  std::endl;
}


/// Test different query types
int main(int, char* [])
{
    JsonioSettings::settingsFileName = "jsonio-config.json";

    ioSettings().get_logger("new_logger");
    ioSettings().set_levels("trace");
    ioSettings().set_module_level("jsonarango", "info");
    ioSettings().set_module_level("other_logger", "debug");

    // Test collection name
    std::string collectionName = "test";

    // Select from number of documents
    int numOfDocuments =  10;

    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;

    try{
        // Connect to Arangodb ( load settings from "jsonio-config.json" config file )
        DataBase db;
        // Open collection, if document collection collectionName not exist it would be created
        auto coll= db.collection( collectionName );

        // Insert documents to database
        for( int ii=0; ii<numOfDocuments; ii++ )
        {
            auto jsFree = JsonFree::object();
            jsFree["name"] = (  ii%2 ? "a" : "b" );
            jsFree["index"] = ii;
            jsFree["task"] = "exampleQuery";
            jsFree["properties"]["value"] = 10.01*ii;
            auto rkey = coll->createDocument( jsFree );
            recKeys.push_back(rkey);
        }


        // Define call back function
        SetReadedKey_f setfnc_key = [&recjsonValues]( const std::string& jsondata, const std::string& )
        {
            recjsonValues.push_back(jsondata);
        };


        // Select all records
        recjsonValues.clear();
        coll->allQuery( {}, setfnc_key );
        printData( "Select all records 1 ", recjsonValues );

        recjsonValues.clear();
        std::set<std::string> fields = { "_id", "name", "index"};
        coll->allQuery( fields, setfnc_key );
        printData( "Select all records 2 ", recjsonValues );

        // Define call back function
        SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
        {
            recjsonValues.push_back(jsondata);
        };


        // Select records by template
        recjsonValues.clear();
        DBQueryBase    templatequery( "{ \"name\" : \"a\" }", DBQueryBase::qTemplate );
        coll->selectQuery( templatequery, setfnc );
        io_logger->trace("Select records by template !!!!!!!!!!!!");
        printData( "Select records by template", recjsonValues );

        // Select records by AQL query
        recjsonValues.clear();
        std::string aql = "FOR u IN " + collectionName +
                "\nFILTER u.properties.value > 50 \n"
                "RETURN { \"_id\": u._id, \"name\":u.name, \"index\":u.index }";
        DBQueryBase    aqlquery( aql, DBQueryBase::qAQL );
        coll->selectQuery( aqlquery, setfnc );
        printData( "Select records by AQL query", recjsonValues );

        // delete all
        coll->removeByKeys( recKeys );
        recjsonValues.clear();
        coll->selectQuery(  DBQueryBase(), setfnc );
        printData( "All after removing", recjsonValues );

        std::cout <<  "Finish test " <<  std::endl;

    }
    catch(jsonio_exception& e)
    {
        std::cout <<   e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<   "std::exception: " << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  "unknown exception" <<  std::endl;
    }

    return 0;
 }
