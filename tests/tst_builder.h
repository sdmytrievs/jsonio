#pragma once

#include <gtest/gtest.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsonio17/service.h"
#include "jsonio17/jsonbuilder.h"
#include "jsonio17/jsondump.h"
#include "jsonio17/jsonfree.h"

using namespace testing;
using namespace jsonio17;

TEST( JsonioBuilder, addScalar )
{
    auto jsFree = JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addScalar( "vnull", "null").addScalar( "vbool1", "true" )
            .addScalar( "vbool2", "false" ).addScalar( "vint", "3" )
            .addScalar( "vdouble", "1e-10" ).addScalar( "vstring", "Test String" );

    EXPECT_EQ( jsFree.size(), 6 );
    EXPECT_TRUE( jsFree["vnull"].isNull() );

    EXPECT_TRUE( jsFree["vbool1"].isBool() );
    EXPECT_TRUE( jsFree["vbool1"].toBool() );
    EXPECT_TRUE( jsFree["vbool2"].isBool() );
    EXPECT_FALSE( jsFree["vbool2"].toBool() );

    EXPECT_TRUE( jsFree["vint"].isNumber() );
    EXPECT_EQ( jsFree["vint"].toInt(), 3 );

    EXPECT_TRUE( jsFree["vdouble"].isNumber() );
    EXPECT_EQ( jsFree["vdouble"].toDouble(), 1e-10 );

    EXPECT_TRUE( jsFree["vstring"].isString() );
    EXPECT_EQ( jsFree["vstring"].toString(), "Test String" );
}

TEST( JsonioBuilder, JsonObjectBuilder )
{
    auto jsFree = JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addNull( "vnull").addBool( "vbool1", true )
            .addBool( "vbool2", false ).addInt( "vint", 3 )
            .addDouble( "vdouble", 1e-10 ).addString( "vstring", "Test String" );

    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 6 );
    EXPECT_TRUE( jsFree["vnull"].isNull() );

    EXPECT_TRUE( jsFree["vbool1"].isBool() );
    EXPECT_TRUE( jsFree["vbool1"].toBool() );
    EXPECT_TRUE( jsFree["vbool2"].isBool() );
    EXPECT_FALSE( jsFree["vbool2"].toBool() );

    EXPECT_TRUE( jsFree["vint"].isNumber() );
    EXPECT_EQ( jsFree["vint"].toInt(), 3 );

    EXPECT_TRUE( jsFree["vdouble"].isNumber() );
    EXPECT_EQ( jsFree["vdouble"].toDouble(), 1e-10 );

    EXPECT_TRUE( jsFree["vstring"].isString() );
    EXPECT_EQ( jsFree["vstring"].toString(), "Test String" );
}

TEST( JsonioBuilder, JsonValueObjectBuilder )
{
    int  vint = 3;
    float vfloat = 2.5;
    double vdouble = 1e-10;
    std::vector<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::map<std::string, std::string> vumaps = { {"key1", "val1" }, {"key2", "val2" } };

    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addValue( "vint", vint );
    jsBuilder.addValue( "vdouble", vdouble );
    jsBuilder.addValue( "vfloat", vfloat );
    jsBuilder.addVector( "vlist", vlist );
    jsBuilder.addMapKey( "vumap", vumaps );

    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 5 );

    EXPECT_TRUE( jsFree["vint"].isNumber() );
    EXPECT_EQ( jsFree["vint"].toInt(), vint );
    EXPECT_TRUE( jsFree["vdouble"].isNumber() );
    EXPECT_EQ( jsFree["vdouble"].toDouble(), vdouble );
    EXPECT_TRUE( jsFree["vfloat"].isNumber() );
    EXPECT_EQ( jsFree["vfloat"].toDouble(),vfloat );

    EXPECT_TRUE( jsFree["vlist"].isArray() );
    EXPECT_EQ( jsFree["vlist"].size(), 4 );
    auto restr = jsFree["vlist"].toString(true);
    trim( restr );
    EXPECT_EQ( restr, "[1.7,2.7,3.7,4.7]" );

    EXPECT_TRUE( jsFree["vumap"].isObject() );
    EXPECT_EQ( jsFree["vumap"].size(), 2 );
    restr = jsFree["vumap"].toString(true);
    trim( restr );
    EXPECT_EQ(restr, "{\"key1\":\"val1\",\"key2\":\"val2\"}" );
}

TEST( JsonioBuilder, JsonComplexObjectBuilder )
{
    auto jsFree = JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addString( "name", "ComplexObject" );

    auto arr = jsBuilder.addArray("array");
    for( int ii=0; ii<10; ++ii )
    {
        arr.addValue(ii);
    }

    auto obj = jsBuilder.addObject("object").addInt("version", 1).addString("comment", "Test example");

    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 3 );

    EXPECT_TRUE( jsFree["name"].isString() );
    EXPECT_EQ( jsFree["name"].size(), 0 );
    EXPECT_EQ( jsFree["name"].toString(), "ComplexObject" );

    EXPECT_TRUE( jsFree["array"].isArray() );
    EXPECT_EQ( jsFree["array"].size(), 10 );
    auto restr = jsFree["array"].toString(true);
    trim( restr );
    EXPECT_EQ( restr, "[0,1,2,3,4,5,6,7,8,9]" );

    EXPECT_TRUE( jsFree["object"].isObject() );
    EXPECT_EQ( jsFree["object"].size(), 2 );
    restr = jsFree["object"].toString(true);
    trim( restr );
    EXPECT_EQ( restr, "{\"version\":1,\"comment\":\"Test example\"}" );
}


//-----------------------------------------------------------------------------------------------

TEST( JsonioBuilder, JsonArrayBuilder )
{
    auto jsFree = JsonFree::array();
    JsonArrayBuilder jsBuilder(&jsFree);

    jsBuilder.addNull( ).addBool( true )
            .addBool( false ).addInt( 3 )
            .addDouble( 1e-10 ).addString( "Test String" );

    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 6 );
    EXPECT_TRUE( jsFree[0].isNull() );

    EXPECT_TRUE( jsFree[1].isBool() );
    EXPECT_TRUE( jsFree[1].toBool() );
    EXPECT_TRUE( jsFree[2].isBool() );
    EXPECT_FALSE( jsFree[2].toBool() );

    EXPECT_TRUE( jsFree[3].isNumber() );
    EXPECT_EQ( jsFree[3].toInt(), 3 );

    EXPECT_TRUE( jsFree[4].isNumber() );
    EXPECT_EQ( jsFree[4].toDouble(), 1e-10 );

    EXPECT_TRUE( jsFree[5].isString() );
    EXPECT_EQ( jsFree[5].toString(), "Test String" );
    //EXPECT_THROW( jsFree["test"], jarango_exception );
}


TEST( JsonioBuilder, JsonIntArrayBuilder )
{
    auto jsFree = JsonFree::array();
    JsonArrayBuilder jsBuilder(&jsFree);

    for( size_t ii=0; ii<5; ++ii )
        jsBuilder.addValue( ii );

    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );

    EXPECT_TRUE( jsFree[0].isNumber() );
    EXPECT_EQ( jsFree[0].toInt(), 0 );

    EXPECT_TRUE( jsFree[4].isNumber() );
    EXPECT_EQ( jsFree[4].toInt(), 4 );
}

TEST( JsonioBuilder, JsonComplexArrayBuilder )
{
    auto jsFree = JsonFree::array();
    JsonArrayBuilder jsBuilder(&jsFree);

    jsBuilder.addString( "ComplexObject" );

    auto arr = jsBuilder.addArray();
    for( int ii=0; ii<10; ++ii )
    {
        arr.addValue(ii);
    }

    auto obj = jsBuilder.addObject().addInt("version", 1).addString("comment", "Test example");

    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 3 );

    EXPECT_TRUE( jsFree[0].isString() );
    EXPECT_EQ( jsFree[0].size(), 0 );
    EXPECT_EQ( jsFree[0].toString(), "ComplexObject" );

    EXPECT_TRUE( jsFree[1].isArray() );
    EXPECT_EQ( jsFree[1].size(), 10 );
    auto restr = jsFree[1].toString(true);
    trim( restr );
    EXPECT_EQ( restr, "[0,1,2,3,4,5,6,7,8,9]" );

    EXPECT_TRUE( jsFree[2].isObject() );
    EXPECT_EQ( jsFree[2].size(), 2 );
    restr = jsFree[2].toString(true);
    trim( restr );
    EXPECT_EQ( restr, "{\"version\":1,\"comment\":\"Test example\"}" );
}

TEST( JsonioBuilder, JsonValueArrayBuilder )
{
    int  vint = 3;
    float vfloat = 2.5;
    double vdouble = 1e-10;
    std::vector<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::map<std::string, std::string> vumaps = { {"key1", "val1" }, {"key2", "val2" } };

    auto jsFree =JsonFree::array();
    JsonArrayBuilder jsBuilder(&jsFree);

    jsBuilder.addValue( vint );
    jsBuilder.addValue( vdouble );
    jsBuilder.addValue( vfloat );
    jsBuilder.addVector( vlist );
    jsBuilder.addMapKey( vumaps );

    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );

    EXPECT_TRUE( jsFree[0].isNumber() );
    EXPECT_EQ( jsFree[0].toInt(), vint );
    EXPECT_TRUE( jsFree[1].isNumber() );
    EXPECT_EQ( jsFree[1].toDouble(), vdouble );
    EXPECT_TRUE( jsFree[2].isNumber() );
    EXPECT_EQ( jsFree[2].toDouble(),vfloat );

    EXPECT_TRUE( jsFree[3].isArray() );
    EXPECT_EQ( jsFree[3].size(), 4 );
    auto restr = jsFree[3].toString(true);
    trim( restr );
    EXPECT_EQ( restr, "[1.7,2.7,3.7,4.7]" );

    EXPECT_TRUE( jsFree[4].isObject() );
    EXPECT_EQ( jsFree[4].size(), 2 );
    restr = jsFree[4].toString(true);
    trim( restr );
    EXPECT_EQ(restr, "{\"key1\":\"val1\",\"key2\":\"val2\"}" );
}
