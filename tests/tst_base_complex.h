#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <list>
#include <map>
#include <unordered_map>
#include "jsonfree.h"

using namespace testing;
using namespace jsonio14;


template <typename T>
class JsonioBaseComplexTest : public ::testing::Test
{

public:

    const std::string schemaName = "ComplexSchemaTest";
    const std::string input_json = "{\"about\":{\"version\":1,\"description\":\"About\"},\"formats\":"
                                   "{\"int\":{\"width\":5,\"precision\":0},\"float\":{\"width\":10,\"precision\":4},"
                                   "\"double\":{\"width\":15,\"precision\":6}},\"data\":[{\"group\":\"float\",\"value\":1.4},"
                                   "{\"group\":\"int\",\"value\":100},{\"group\":\"double\",\"value\":1e-10},{\"group\":\"double\",\"value\":10000000000}],"
                                   "\"values\":[[1,2,3],[11,12,13]]}";

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

using JsonTypes = ::testing::Types<JsonFree>;
TYPED_TEST_SUITE(JsonioBaseComplexTest, JsonTypes);

TYPED_TEST( JsonioBaseComplexTest, GetPath )
{
    const auto& obj = *this->test_object;

    EXPECT_EQ( "about.description", obj["about"]["description"].get_path());
    EXPECT_EQ( "formats.float.width", obj["formats"]["float"]["width"].get_path());
    EXPECT_EQ( "data.1.value", obj["data"][1]["value"].get_path());
    EXPECT_EQ( "values.0.2", obj["values"][0][2].get_path());

    /*EXPECT_TRUE( obj.isTop());
    EXPECT_FALSE( obj["vbool"].isTop() );
    EXPECT_FALSE( obj["vlist"].isTop() );
    EXPECT_FALSE( obj["vmap"]["key1"].isTop() );*/
}

TYPED_TEST( JsonioBaseComplexTest, isStructured1 )
{
    const auto& obj = *this->test_object;
    /*EXPECT_TRUE( obj.isStructured());

    EXPECT_TRUE( obj["vlist"].isStructured() );
    EXPECT_TRUE( obj["vmap"].isStructured() );
    EXPECT_TRUE( obj["vlist"].isArray() );
    EXPECT_TRUE( obj["vmap"].isObject() );

    EXPECT_FALSE( obj["vbool"].isStructured() );
    EXPECT_FALSE( obj["vmap"]["key1"].isStructured() );*/
}
