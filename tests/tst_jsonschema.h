#pragma once

#include <gtest/gtest.h>

#include <list>
#include <map>
#include <unordered_map>
#include "jsonio14/jsondump.h"
#include "jsonio14/jsonschema.h"
#include "jsonio14/io_settings.h"
#include "jsonio14/schema_thrift.h"
#include "example_schema.h"

using namespace testing;
using namespace jsonio14;

TEST( JsonioSchemaJson, ObjectLoads)
{
    ioSettings().addSchemaFormat(schema_thrift, schema_str);
    auto obj = json::loads( "ComplexSchemaTest", complex_schema_value );

    EXPECT_EQ( obj["about"].dump(true), "{\"version\":1,\"description\":\"About\"}\n" );
    EXPECT_EQ( obj["formats"].dump(true),
            "{\"int\":{\"width\":5,\"precision\":0},\"float\":{\"width\":10,\"precision\":4},\"double\":{\"width\":15,\"precision\":6}}\n" );
    EXPECT_EQ( obj["data"].dump(true), "[{\"group\":\"float\",\"value\":1.4},"
                                       "{\"group\":\"int\",\"value\":100},{\"group\":\"double\",\"value\":1e-10},{\"group\":\"double\",\"value\":10000000000}]\n" );
    EXPECT_EQ( obj["values"].dump(true), "[[1,2,3],[11,12,13]]\n" );
}


TEST( JsonioSchemaJson, ObjectSimple)
{
    ioSettings().addSchemaFormat(schema_thrift, schema_str);

    int  vint{15};
    std::vector<double> vlist{ 17, 27 };
    std::map<std::string, std::string> vumap{ {"key1", "val1" }, {"key2", "val2" } };
    std::string vstr{"New string"};

    auto simple_object = JsonSchema( JsonSchema::object("SimpleSchemaTest") );
    EXPECT_EQ( simple_object.dump(true), "{}\n" );

    simple_object["vstring"] =  vstr;
    simple_object["vlist"] = vlist;
    simple_object["vlist"][2] = 11;
    simple_object["vmap"] = vumap;

    EXPECT_EQ( simple_object.dump(true), "{\"vstring\":\"New string\",\"vlist\":[17,27,11],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}\n" );

    simple_object["vmap"]["key3"] = "10";
    simple_object["vbool"] = true;
    simple_object["vint"] = vint;
    simple_object["vdouble"] = 2.5;   // order test

    EXPECT_EQ( simple_object.dump(true), "{\"vbool\":true,\"vint\":15,\"vdouble\":2.5,\"vstring\":\"New string\","
                                         "\"vlist\":[17,27,11],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\",\"key3\":\"10\"}}\n" );

    // exception undefined field
    EXPECT_THROW( simple_object["no_exist"], jsonio_exception );
    // exception not next index
    EXPECT_THROW( simple_object["vlist"][10], jsonio_exception );
}


TEST( JsonioSchemaJson, ObjectComplex)
{
    ioSettings().addSchemaFormat(schema_thrift, schema_str);

    auto complex_object = JsonSchema( JsonSchema::object("ComplexSchemaTest") );
    // se6t default
    EXPECT_EQ(  complex_object.dump(true), "{\"about\":{\"version\":1},\"values\":[[1,2],[3,4]]}\n" );

    auto describe_object = JsonSchema( JsonSchema::object("Describe") );
    describe_object["description"] = "the description";

    auto spdata_object = JsonSchema( JsonSchema::object("SpecifiersData") );
    spdata_object["group"] = "the group";
    // with default
    EXPECT_EQ(  spdata_object.dump(true), "{\"group\":\"the group\",\"value\":100000}\n" );

    auto format_object = JsonSchema( JsonSchema::object("FormatData") );
    format_object["width"] = 10;
    format_object["precision"] = 8;
    EXPECT_EQ(  format_object.dump(true) , "{\"width\":10,\"precision\":8}\n" );

    complex_object["about"] = describe_object;
    complex_object["formats"]["new"] = format_object;
    complex_object["data"][0] =spdata_object;

    EXPECT_EQ(  complex_object.dump(true) , "{\"about\":{\"description\":\"the description\"},"
                                            "\"formats\":{\"new\":{\"width\":10,\"precision\":8}},"
                                            "\"data\":[{\"group\":\"the group\",\"value\":100000}],"
                                            "\"values\":[[1,2],[3,4]]}\n" );
}


TEST( JsonioSchemaJson, fieldOidPath )
{
    ioSettings().addSchemaFormat(schema_thrift, schema_str);
    auto obj = json::loads( "ComplexSchemaTest", complex_schema_value );

    std::string sval;
    EXPECT_TRUE( obj.get_value_via_path( "about.description", sval, std::string("undef") ) );
    EXPECT_EQ( sval, "About");
    EXPECT_TRUE( obj.get_value_via_path( "1.4", sval, std::string("undef") ) );
    EXPECT_EQ( sval, "About");
    double dval;
    EXPECT_TRUE( obj.get_value_via_path( "formats.float.width", dval, 1.5 ) );
    EXPECT_EQ( dval, 10);
    EXPECT_TRUE( obj.get_value_via_path( "2.1.1", dval, 1.5 ) );
    EXPECT_EQ( dval, 10);
    EXPECT_TRUE( obj.get_value_via_path( "data.1.value", dval, 1.5 ) );
    EXPECT_EQ( dval, 100);
    EXPECT_TRUE( obj.get_value_via_path( "3.1.2", dval, 1.5 ) );
    EXPECT_EQ( dval, 100);
    EXPECT_FALSE( obj.get_value_via_path( "data.4.value", dval, 1.5 ) );
    EXPECT_EQ( dval, 1.5);
    EXPECT_FALSE( obj.get_value_via_path( "3.4.2", dval, 1.5 ) );
    EXPECT_EQ( dval, 1.5);
}

