#pragma once

#include <gtest/gtest.h>

#include <list>
#include <map>
#include <unordered_map>
#include "jsonio/jsonfree.h"
#include "jsonio/jsonschema.h"
#include "jsonio/io_settings.h"
#include "jsonio/schema_thrift.h"
#include "example_schema.h"

using namespace testing;
using namespace jsonio;


template <typename T>
class JsonioBaseComplexTest : public ::testing::Test
{

public:

    const std::string schemaName = "ComplexSchemaTest";
    const std::string input_json = complex_schema_value;
    //    "{\"about\":{\"version\":1,\"description\":\"About\"},\"formats\":"
    //    "{\"int\":{\"width\":5,\"precision\":0},\"float\":{\"width\":10,\"precision\":4},"
    //    "\"double\":{\"width\":15,\"precision\":6}},\"data\":[{\"group\":\"float\",\"value\":1.4},"
    //    "{\"group\":\"int\",\"value\":100},{\"group\":\"double\",\"value\":1e-10},{\"group\":\"double\",\"value\":10000000000}],"
    //    "\"values\":[[1,2,3],[11,12,13]]}";

    virtual void SetUp()
    { }

    virtual void TearDown()
    {
        delete test_object;
    }

    T* test_object = nullptr;

};

template<> void JsonioBaseComplexTest<JsonFree>::SetUp()
{
    test_object = new  JsonFree( JsonFree::object() );
    test_object->loads( input_json );
}

template<> void JsonioBaseComplexTest<JsonSchema>::SetUp()
{
    ioSettings().addSchemaFormat(schema_thrift, schema_str);
    test_object = new  JsonSchema( JsonSchema::object(schemaName) );
    test_object->loads( input_json );
    //std::cout << test_object->dump(false);
}

using JsonTypes2 = ::testing::Types<JsonFree, JsonSchema>;
TYPED_TEST_SUITE(JsonioBaseComplexTest, JsonTypes2);

TYPED_TEST( JsonioBaseComplexTest, GetPath )
{
    const auto& obj = *this->test_object;

    EXPECT_EQ( "about.description", obj["about"]["description"].get_path());
    EXPECT_EQ( "formats.float.width", obj["formats"]["float"]["width"].get_path());
    EXPECT_EQ( "data.1.value", obj["data"][1]["value"].get_path());
    EXPECT_EQ( "values.0.2", obj["values"][0][2].get_path());

    EXPECT_TRUE( obj.path_if_exists( "formats.float.width" ) );
    EXPECT_TRUE( obj.path_if_exists( "/formats/int/width" ) );
    EXPECT_TRUE( obj.path_if_exists( "data[1].value" ) );
    EXPECT_TRUE( obj.path_if_exists( "values[0][2]" ) );
    EXPECT_TRUE( obj.path_if_exists( "about" ) );

    EXPECT_FALSE( obj.path_if_exists( "formats.float.other" ) );
    EXPECT_FALSE( obj.path_if_exists( "data[1].value1" ) );
    EXPECT_FALSE( obj.path_if_exists( "values[2][2]" ) );
    EXPECT_FALSE( obj.path_if_exists( "about.version1" ) );
}

TYPED_TEST( JsonioBaseComplexTest, ValueViaPath )
{
    auto& obj = *this->test_object;
    double dval;
    EXPECT_TRUE( obj.get_value_via_path( "formats.float.width", dval, 1.5 ) );
    EXPECT_EQ( dval, 10);
    EXPECT_TRUE( obj.get_value_via_path( "data[1].value", dval, 1.5 ) );
    EXPECT_EQ( dval, 100);
    EXPECT_FALSE( obj.get_value_via_path( "data[4].value", dval, 1.5 ) );
    EXPECT_EQ( dval, 1.5);

    dval = 2.5;
    EXPECT_TRUE( obj.set_value_via_path( "data[1].value", dval ) );
    EXPECT_EQ( obj["data"][1]["value"].toDouble(), dval );
    EXPECT_FALSE( obj.set_value_via_path( "data[5].value", dval ) );
    EXPECT_TRUE( obj.set_value_via_path( "data[4].value", dval ) );

    std::string sval;
    EXPECT_TRUE( obj.get_value_via_path( "data[2]", sval, std::string("undefined") ) );
    EXPECT_EQ( sval, "{\"group\":\"double\",\"value\":1e-10}");

    EXPECT_TRUE( obj.get_key_via_path( "data[2]", sval ) );
    EXPECT_EQ( sval, "");
}

TYPED_TEST( JsonioBaseComplexTest, AddValueViaPath )
{
    auto& obj = *this->test_object;

    int iwidth = 10;
    EXPECT_TRUE( obj.set_value_via_path( "formats.add_object2.width", iwidth ) );
    EXPECT_TRUE( obj["formats"]["add_object2"].isObject() );
    EXPECT_FALSE( obj["formats"]["add_object2"].empty() );
    EXPECT_EQ( obj["formats"]["add_object2"]["width"].toInt(), iwidth );

    EXPECT_FALSE( obj.set_value_via_path( "data[7].value", 2.5 ) );
    EXPECT_TRUE( obj.set_value_via_path( "data[4].value", 2.5 ) );
}


TYPED_TEST( JsonioBaseComplexTest, ClearRemove )
{
    auto& obj = *this->test_object;

    EXPECT_TRUE( obj["formats"]["float"]["width"].clear() );
    EXPECT_EQ( obj["formats"]["float"]["width"].toDouble(), 0 );

    EXPECT_TRUE( obj["about"]["description"].clear() );
    EXPECT_EQ( obj["about"]["description"].toString(), "" );

    EXPECT_TRUE( obj["formats"]["float"].clear() );
    EXPECT_EQ( obj["formats"]["float"].toString(true), "{}" );

    EXPECT_FALSE( obj.remove() );

    //EXPECT_EQ( obj["about"].toString(true), "{\"version\":1,\"description\":\"\"}\n" );
    EXPECT_TRUE( obj["about"]["version"].remove() );
    //EXPECT_EQ( obj["about"].toString(true), "{\"description\":\"\"}\n" );
    EXPECT_TRUE( obj.path_if_exists( "about" ) );
    EXPECT_FALSE( obj.path_if_exists( "about.version" ) );

    EXPECT_EQ( obj["data"].size(), static_cast<size_t>(4) );
    EXPECT_TRUE( obj["data"][1].remove() );
    EXPECT_EQ( obj["data"].size(), static_cast<size_t>(3) );
    EXPECT_EQ( obj["data"].toString(true),
            "[{\"group\":\"float\",\"value\":1.4},{\"group\":\"double\",\"value\":1e-10},{\"group\":\"double\",\"value\":10000000000}]" );

}

TYPED_TEST( JsonioBaseComplexTest, ArrayResize )
{
    auto& obj = *this->test_object;

    auto sizes = obj["values"].array_sizes();
    EXPECT_EQ( static_cast<size_t>(2), sizes.size() );
    EXPECT_EQ( static_cast<size_t>(2), sizes[0] );
    EXPECT_EQ( static_cast<size_t>(3), sizes[1] );

    obj["values"].array_resize_xD({3,5}, "5");
    EXPECT_EQ( obj["values"].dump(true), "[[1,2,3,5,5],[11,12,13,5,5],[1,2,3,5,5]]" );
    obj["values"].array_resize_xD({4,3}, "5");
    EXPECT_EQ( obj["values"].dump(true), "[[1,2,3],[11,12,13],[1,2,3],[1,2,3]]" );
    obj["values"].array_resize_xD({2,2}, "");
    EXPECT_EQ( obj["values"].dump(true), "[[1,2],[11,12]]" );
}


