#include <iostream>
#include <chrono>

/// Time test example for ArangoDBCollectionAPI the API for manipulating collections and documents into.

#include <fstream>
#include "jsonio/jsonfree.h"
//#include "jsonio/dbcollection.h"
#include "jsonio/dbvertexdoc.h"
#include "jsonio/dbquerybase.h"
#include "jsonio/io_settings.h"
using namespace jsonio;


using time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

// Current number documents into collection
static int documentsInCollection =  10;

void printData( const std::string&  title, const std::vector<std::string>& values );
void printTime( const std::string&  title, const time_point_t& start, const time_point_t& end );
void printTimeSec( const std::string&  title, const time_point_t& start, const time_point_t& end );

void printData( const std::string&  title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  std::endl;
    for( const auto& jsondata: values)
        std::cout <<  jsondata <<  std::endl;
    std::cout <<  std::endl;
}


void printTime( const std::string&  title, const time_point_t& start, const time_point_t& end )
{
    std::cout <<  title << "\nElapsed time in microseconds: ";
    std::cout <<  std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << " μs" << std::endl;
}

void printTimeSec( const std::string&  title, const time_point_t& start, const time_point_t& end )
{
    std::cout <<  title << "\nElapsed time in seconds: ";
//    std::cout <<  std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << "s" << std::endl;
    std::cout <<  std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ms" << std::endl;
}

/// Test different query types
int different_query_types( DataBase& connect );
int substances_query_types( DataBase& connect );
int substances_vertex( DataBase& connect );


int main(int argc, char* argv[])
{
    JsonioSettings::settingsFileName = "jsonio-config.json";

    if( argc > 1)
        documentsInCollection = std::stoi(argv[1]);

    if( argc > 2)
        JsonioSettings::settingsFileName = argv[2];

    try{

        // Connect to Arangodb ( load settings from "jsonio-config.json" config file )
        DataBase db;

        //different_query_types( db );
        //substances_query_types( db );
        substances_vertex( db );

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

// Test different query types
int different_query_types( DataBase& connect )
{
    // Test collection name
    std::string collectionName = "test";
    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;
    // Define call back function
    SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        recjsonValues.push_back(jsondata);
    };

    auto start = std::chrono::high_resolution_clock::now();

    auto coll= connect.collection( collectionName );

    // Open collection, if document collection collectionName not exist it would be created
    auto end1 = std::chrono::high_resolution_clock::now();
    printTime( "Create collection", start, end1 );

    // Insert documents to database
    for( int ii=0; ii<documentsInCollection; ii++ )
    {
        auto jsFree = JsonFree::object();
        jsFree["name"] = (  ii%2 ? "a" : "b" );
        jsFree["index"] = ii;
        jsFree["task"] = "exampleQuery";
        jsFree["properties"]["value"] = 10.01*ii;
        auto rkey = coll->createDocument( jsFree );
        recKeys.push_back(rkey);
    }

    auto end2 = std::chrono::high_resolution_clock::now();
    printTime( "Insert documents to database", end1, end2 );

    // Select all records
    recjsonValues.clear();
    coll->selectQuery( DBQueryBase(DBQueryBase::qAll), setfnc );
    //printData( "Select all records", recjsonValues );
    auto end3 = std::chrono::high_resolution_clock::now();
    printTime( "Select all records", end2, end3 );

    // Select records by template
    recjsonValues.clear();
    DBQueryBase    templatequery( "{ \"name\" : \"a\" }", DBQueryBase::qTemplate );
    coll->selectQuery( templatequery, setfnc );
    //printData( "Select records by template", recjsonValues );
    auto end4 = std::chrono::high_resolution_clock::now();
    printTime( "Select records by template", end3, end4 );

    // Select records by AQL query
    recjsonValues.clear();
    std::string aql = "FOR u IN " + collectionName +
            "\nFILTER u.properties.value > 50 \n"
            "RETURN { \"_id\": u._id, \"name\":u.name, \"index\":u.index }";
    DBQueryBase    aqlquery( aql, DBQueryBase::qAQL );
    coll->selectQuery( aqlquery, setfnc );
    //printData( "Select records by AQL query", recjsonValues );
    auto end5 = std::chrono::high_resolution_clock::now();
    printTime( "Select records by AQL query", end4, end5 );

    // delete all
    coll->removeByKeys( recKeys );
    auto end6 = std::chrono::high_resolution_clock::now();
    printTime( "Delete by keys", end5, end6 );

    printTime( "All time", start, end6 );

    return 0;
}

/*
You are connected to: arango 3.6.1
Create collection
Elapsed time in microseconds: 147365 μs
Insert documents to database
Elapsed time in microseconds: 6771490 μs
Select all records
Elapsed time in microseconds: 69230 μs
Select records by template
Elapsed time in microseconds: 72953 μs
Select records by AQL query
Elapsed time in microseconds: 65737 μs
Delete by keys
Elapsed time in microseconds: 205997 μs
All time
Elapsed time in microseconds: 7332774 μs

*/

int substances_query_types( DataBase& connect )
{
    // Test collection name
    std::string collectionName = "substances";
    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;
    // Define call back function
    SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        recjsonValues.push_back(jsondata);
    };
    // Define call back function
    SetReadedKey_f setfnc_key = [&recjsonValues]( const std::string& jsondata, const std::string& )
    {
        recjsonValues.push_back(jsondata);
    };

    auto start = std::chrono::high_resolution_clock::now();

    // If document collection collectionName not exist it would be created
    auto coll= connect.collection( collectionName );

    auto end1 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Create collection", start, end1 );

    // Select all records
    recjsonValues.clear();
    coll->allQuery( {}, setfnc_key );
    //printData( "Select all records 1 ", recjsonValues );
    auto end2 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select all records ( " + std::to_string(recjsonValues.size()) + " )", end1, end2 );

    recjsonValues.clear();
    std::set<std::string> fields = { "_id", "_label" };
    coll->allQuery( fields, setfnc_key );
    //printData( "Select all records 2 ", recjsonValues );
    auto end3 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select all records with fields ( " + std::to_string(recjsonValues.size()) + " )", end2, end3 );

    // Select records by template
    recjsonValues.clear();
    DBQueryBase    templatequery( "{ \"_label\" : \"substance\" }", DBQueryBase::qTemplate );
    coll->selectQuery( templatequery, setfnc );
    //printData( "Select records by template", recjsonValues );
    auto end4 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select records by template ( " + std::to_string(recjsonValues.size()) + " )", end3, end4 );

    // Select records by AQL query
    recjsonValues.clear();
    std::string aql = "FOR u IN " + collectionName +
            // "\nFILTER u.properties.value > 50 \n"
            "\nRETURN { \"_id\": u._id, \"name\":u.properties.name }";
    DBQueryBase    aqlquery( aql, DBQueryBase::qAQL );
    coll->selectQuery( aqlquery, setfnc );
    //printData( "Select records by AQL query", recjsonValues );
    auto end5 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select records by AQL query ( " + std::to_string(recjsonValues.size()) + " )", end4, end5 );

    printTimeSec( "All time", start, end5 );
    return 0;
}

/*
You are connected to: arango 3.6.1
Create collection
Elapsed time in seconds: 1526ms
Select all records ( 6003 )
Elapsed time in seconds: 2094ms
Select all records with fields ( 6003 )
Elapsed time in seconds: 994ms
Select records by template ( 6003 )
Elapsed time in seconds: 1971ms
Select records by AQL query ( 6003 )
Elapsed time in seconds: 975ms
All time
Elapsed time in seconds: 7562ms



*/

int substances_vertex( DataBase& connect )
{
    // Test collection name
    std::string collectionName = "substances";
    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;
    // Define call back function
    SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        //std::cout << jsondata << std::endl;
        recjsonValues.push_back(jsondata);
    };

    auto start = std::chrono::high_resolution_clock::now();

    // If document collection collectionName not exist it would be created
//    auto vertex_doc =  std::shared_ptr<DBVertexDocument>(
//                DBVertexDocument::newVertexDocument( connect, "VertexSubstance" ));
    auto vertex_doc =  std::shared_ptr<DBVertexDocument>(
                DBVertexDocument::newVertexDocumentQuery( connect, "VertexSubstance" ));

    auto end1 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Create collection", start, end1 );

//    std::fstream table_fout("subst_table_part.csv", std::ios::out );
//    auto sub_table = vertex_doc->currentQueryResult().queryResult();
//    for( const auto& line: sub_table)
//    {
//      table_fout  << line.first << "; ";
//      for( const auto& col: line.second )
//          table_fout  << col << "; ";
//      table_fout  << "\n";
//    }
//    return 0;
    //vertex_doc->readDocument("substances/methionine,cr;0:SC_COMPONENT;23:SLOP16");
    //std::cout << vertex_doc->getJson(true) << std::endl;

    std::cout << " !!! out " << vertex_doc->existOutEdges("substances/H2O;1:SC_GASFLUID;18:MINES") << std::endl;
    std::cout << " !!! in " << vertex_doc->existInEdges("substances/(NpO2)2(OH)2+2;2:SC_AQSOLUTE;15:PSI_NAGRA") << std::endl;

    // Select records by template
    recjsonValues.clear();
    DBQueryBase    templatequery( "{ \"_label\" : \"substance\" }", DBQueryBase::qTemplate );
    vertex_doc->selectQuery( templatequery, setfnc );
    //printData( "Select records by template", recjsonValues );
    auto end2 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select records by template ( " + std::to_string(recjsonValues.size()) + " )", end1, end2 );

    // Select records by AQL query
    recjsonValues.clear();
    std::string aql = "FOR u IN " + collectionName +
            // "\nFILTER u.properties.value > 50 \n"
            // "\nRETURN { \"_id\": u._id, \"name\":u.properties.name }";
            "\nRETURN u ";
    DBQueryBase    aqlquery( aql, DBQueryBase::qAQL );
    vertex_doc->selectQuery( aqlquery, setfnc );
    //printData( "Select records by AQL query", recjsonValues );
    auto end3 = std::chrono::high_resolution_clock::now();
    printTimeSec( "Select records by AQL query ( " + std::to_string(recjsonValues.size()) + " )", end2, end3 );

    printTimeSec( "All time", start, end3 );
    return 0;
}

/*
You are connected to: arango 3.6.1
loadCollectionFile locked
loadCollectionFile unlocked
VertexSubstance Loding collection 819784 Loding query 206600
Create collection
Elapsed time in seconds: 1026ms


--- threads
You are connected to: arango 3.6.1
loadCollectionFile locked
loadCollectionFile unlocked
VertexSubstance Loding collection 64694 Loding query 159827
Create collection
Elapsed time in seconds: 224ms


*/
