#pragma once

#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>

//https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project


#include "jsonio17/dbquerybase.h"
#include "jsonio17/jsonfree.h"
#include "jsonio17/jsonschema.h"
#include "jsonio17/io_settings.h"
#include "jsonio17/schema_thrift.h"
#include "example_schema.h"
#include "jsonarango/arangoquery.h"
using namespace testing;
using namespace jsonio17;

TEST( DBQuery, testEmpty )
{
    DBQueryBase query(DBQueryBase::qUndef);

    EXPECT_TRUE( query.empty() );
    EXPECT_EQ( query.type(),DBQueryBase::qUndef);
    EXPECT_EQ( query.queryString(),"");
    EXPECT_EQ( query.bindVars(),"");
    EXPECT_EQ( query.options(),"");
    EXPECT_EQ( query.queryFields(), fields2query_t{});


    auto jsFree = JsonFree::object();
    query.toJson(jsFree);
    EXPECT_EQ( jsFree.dump(true),"{\"style\":-1,\"find\":\"\",\"bind\":\"\",\"options\":\"\",\"fields\":{}}\n");

    DBQueryBase query2;
    query2.fromJson(jsFree);
    EXPECT_EQ( query, query2);
}

TEST( DBQuery, testAll )
{
    DBQueryBase query(DBQueryBase::qAll);

    EXPECT_FALSE( query.empty() );
    EXPECT_EQ( query.type(),DBQueryBase::qAll);
    EXPECT_EQ( query.queryString(),"");
    EXPECT_EQ( query.bindVars(),"");
    EXPECT_EQ( query.options(),"");
    EXPECT_EQ( query.queryFields(), fields2query_t{});

    auto jsFree = JsonFree::object();
    query.toJson(jsFree);
    EXPECT_EQ( jsFree.dump(true),"{\"style\":1,\"find\":\"\",\"bind\":\"\",\"options\":\"\",\"fields\":{}}\n");

    DBQueryBase query2(DBQueryBase::qUndef);
    query2.fromJson(jsFree);
    EXPECT_EQ( query, query2);
}

TEST( DBQuery, testTemlate )
{
    DBQueryBase query( "{ \"name\" : \"a\" }", DBQueryBase::qTemplate);

    EXPECT_FALSE( query.empty() );
    EXPECT_EQ( query.type(),DBQueryBase::qTemplate);
    EXPECT_EQ( query.queryString(),"{ \"name\" : \"a\" }");
    EXPECT_EQ( query.bindVars(),"");
    EXPECT_EQ( query.options(),"");
    EXPECT_EQ( query.queryFields(), fields2query_t{});

    auto jsFree = JsonFree::object();
    query.toJson(jsFree);
    EXPECT_EQ( jsFree.dump(true),
          "{\"style\":0,\"find\":\"{ \\\"name\\\" : \\\"a\\\" }\",\"bind\":\"\",\"options\":\"\",\"fields\":{}}\n");

    DBQueryBase query2(DBQueryBase::qUndef);
    query2.fromJson(jsFree);
    EXPECT_EQ( query, query2);
}

TEST( DBQuery, testAQL )
{
    DBQueryBase query( "FOR u IN test RETURN u", DBQueryBase::qAQL);

    EXPECT_FALSE( query.empty() );
    EXPECT_EQ( query.type(),DBQueryBase::qAQL);
    EXPECT_EQ( query.queryString(),"FOR u IN test RETURN u");
    EXPECT_EQ( query.bindVars(),"");
    EXPECT_EQ( query.options(),"");
    EXPECT_EQ( query.queryFields(), fields2query_t{});

    auto jsFree = JsonFree::object();
    query.toJson(jsFree);
    EXPECT_EQ( jsFree.dump(true),
         "{\"style\":6,\"find\":\"FOR u IN test RETURN u\",\"bind\":\"\",\"options\":\"\",\"fields\":{}}\n");

    DBQueryBase query2(DBQueryBase::qUndef);
    query2.fromJson(jsFree);
    EXPECT_EQ( query, query2);
}


TEST( DBQuery, dbQueryDef )
{
    ioSettings().addSchemaFormat( schema_thrift, query_schema_str );

    auto query = std::make_shared<DBQueryBase>(
                "FOR u IN elements\nFILTER u.number IN @nbrs \n",
                DBQueryBase::qAQL);

    std::string bind_vars = "{ \"nbrs\": [1,2,3] }";
    query->setBindVars( bind_vars );
    std::string options = "{ \"maxPlans\" : 1, "
                          "  \"optimizer\" : { \"rules\" : [ \"-all\", \"+remove-unnecessary-filters\" ]  } } ";
    query->setOptions(options);
    fields2query_t map_fields =
    {
      { "properties.symbol", "symbol" },
      { "properties.name", "name" },
      { "properties.class_", "class_" },
      { "properties.number", "number" }
    };
    query->setQueryFields(map_fields);
    EXPECT_EQ( DBQueryBase::generateRETURN( true, map_fields, "u"),
               "\nRETURN DISTINCT  {  \"class_\" : u.properties.class_,  \"name\" : u.properties.name,  \"number\" : u.properties.number,  \"symbol\" : u.properties.symbol } ");
    EXPECT_EQ( DBQueryBase::generateRETURN( false, map_fields, "u"),
               "\nRETURN  {  \"class_\" : u.properties.class_,  \"name\" : u.properties.name,  \"number\" : u.properties.number,  \"symbol\" : u.properties.symbol } ");

    DBQueryDef query_def( query);
    query_def.setName("testAQL");
    query_def.setSchema("VertexElement");
    query_def.setComment("AQL query for elements");
    std::vector<std::string> fields =
              { "properties.symbol",
                "properties.name",
                "properties.class_",
                "properties.number" };
    query_def.setFields(fields);

    EXPECT_EQ( query_def.condition()->type(),DBQueryBase::qAQL);
    EXPECT_EQ( query_def.condition()->queryString(),"FOR u IN elements\nFILTER u.number IN @nbrs \n");
    EXPECT_EQ( query_def.condition()->bindVars(),bind_vars);
    EXPECT_EQ( query_def.condition()->options(),options);
    EXPECT_EQ( query_def.condition()->queryFields(), map_fields);
    EXPECT_EQ( query_def.name(), "testAQL");
    EXPECT_EQ( query_def.schema(), "VertexElement");
    EXPECT_EQ( query_def.comment(), "AQL query for elements");
    EXPECT_EQ( query_def.fields(), fields);

    auto jsonQuery = JsonSchema::object("Query");
    query_def.toJson( jsonQuery );
    //std::cout << jsonQuery.dump() << std::endl;

    DBQueryDef query2(std::make_shared<DBQueryBase>( DBQueryBase::qAll));
    query2.fromJson(jsonQuery);
    EXPECT_EQ( *query_def.condition(), *query2.condition());
    EXPECT_EQ( query2.condition()->type(),DBQueryBase::qAQL);
    EXPECT_EQ( query2.condition()->queryString(),"FOR u IN elements\nFILTER u.number IN @nbrs \n");
    EXPECT_EQ( query2.condition()->bindVars(),bind_vars);
    EXPECT_EQ( query2.condition()->options(),options);
    EXPECT_EQ( query2.condition()->queryFields(), map_fields);
    EXPECT_EQ( query2.name(), "testAQL");
    EXPECT_EQ( query2.schema(), "VertexElement");
    EXPECT_EQ( query2.comment(), "AQL query for elements");
    EXPECT_EQ( query2.fields(), fields);
}


TEST( DBQuery, addFieldsToFilter )
{
    field_value_map_t field_value = {
            { "foo.baz", "1"},
            { "bar", "\"string\""},  };

    DBQueryBase query_all( DBQueryBase::qAll);
    query_all.addFieldsToFilter(field_value);
    EXPECT_EQ( query_all.type(),DBQueryBase::qTemplate);
    EXPECT_EQ( query_all.queryString(),"{  \"bar\" : \"string\" ,  \"foo.baz\" : 1  }");
    EXPECT_EQ( query_all.bindVars(),"");
    EXPECT_EQ( query_all.options(), "");
    EXPECT_EQ( query_all.queryFields(), fields2query_t{} );

    DBQueryBase query_template( "{ \"name\" : \"a\" }", DBQueryBase::qTemplate);
    query_template.addFieldsToFilter(field_value);
    EXPECT_EQ( query_template.type(),DBQueryBase::qTemplate);
    EXPECT_EQ( query_template.queryString(),"{ \"name\" : \"a\" ,  \"bar\" : \"string\" ,  \"foo.baz\" : 1 }");
    EXPECT_EQ( query_template.bindVars(),"");
    EXPECT_EQ( query_template.options(), "");
    EXPECT_EQ( query_template.queryFields(), fields2query_t{} );

    DBQueryBase query_aql( "FOR u IN elements\nFILTER u.number IN @nbrs \n", DBQueryBase::qAQL);

    std::string bind_vars = "{ \"nbrs\": [1,2,3] }";
    query_aql.setBindVars( bind_vars );
    std::string options = "{ \"maxPlans\" : 1, "
                          "  \"optimizer\" : { \"rules\" : [ \"-all\", \"+remove-unnecessary-filters\" ]  } } ";
    query_aql.setOptions(options);
    fields2query_t map_fields =
    {
      { "properties.symbol", "symbol" },
      { "properties.number", "number" }
    };
    query_aql.setQueryFields(map_fields);

    query_aql.addFieldsToFilter(field_value);
    EXPECT_EQ( query_aql.type(),DBQueryBase::qAQL);
    EXPECT_EQ( query_aql.queryString(),"FOR u IN elements\nFILTER u.number IN @nbrs \n\nFILTER u.bar == \"string\" && u.foo.baz == 1 ");
    EXPECT_EQ( query_aql.bindVars(),bind_vars);
    EXPECT_EQ( query_aql.options(),options);
    EXPECT_EQ( query_aql.queryFields(), map_fields);

    DBQueryBase query_aql_return( "FOR u IN elements\nRETURN u", DBQueryBase::qAQL);
    query_aql_return.addFieldsToFilter(field_value);
    EXPECT_EQ( query_aql_return.type(),DBQueryBase::qAQL);
    EXPECT_EQ( query_aql_return.queryString(),"FOR u IN elements\n\nFILTER u.bar == \"string\" && u.foo.baz == 1 \nRETURN u");

}
