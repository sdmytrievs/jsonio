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
class JsonioBaseTest : public ::testing::Test
{

public:

    const std::string schemaName = "SimpleSchemaTest";
    const std::string input_json = simple_schema_value;
    //"{\"vbool\":true,\"vint\":-100,\"vdouble\":5.2,\"vstring\":\"Test string\","
    //"\"vlist\":[1.7,2.7,3.7,5.7],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}";

    virtual void SetUp()
    { }

    virtual void TearDown()
    {
        delete test_object;
    }

    T* test_object = nullptr;

};

template<> void JsonioBaseTest<JsonFree>::SetUp()
{
    test_object = new  JsonFree( JsonFree::object() );
    test_object->loads( input_json );
}

template<> void JsonioBaseTest<JsonSchema>::SetUp()
{
    ioSettings().addSchemaFormat(schema_thrift, schema_str);
    test_object = new  JsonSchema( JsonSchema::object(schemaName) );
    test_object->loads( input_json );
}

using JsonTypes = ::testing::Types<JsonFree, JsonSchema>;

TYPED_TEST_SUITE(JsonioBaseTest, JsonTypes);

TYPED_TEST( JsonioBaseTest, isTop )
{
    const auto& obj = *this->test_object;
    EXPECT_TRUE( obj.isTop());
    EXPECT_FALSE( obj["vbool"].isTop() );
    EXPECT_FALSE( obj["vlist"].isTop() );
    EXPECT_FALSE( obj["vmap"]["key1"].isTop() );
}

TYPED_TEST( JsonioBaseTest, isStructured )
{
    const auto& obj = *this->test_object;
    EXPECT_TRUE( obj.isStructured());

    EXPECT_TRUE( obj["vlist"].isStructured() );
    EXPECT_TRUE( obj["vmap"].isStructured() );
    EXPECT_TRUE( obj["vlist"].isArray() );
    EXPECT_TRUE( obj["vmap"].isObject() );

    EXPECT_FALSE( obj["vbool"].isStructured() );
    EXPECT_FALSE( obj["vmap"]["key1"].isStructured() );
}

TYPED_TEST( JsonioBaseTest, isPrimitive )
{
    const auto& obj = *this->test_object;

    EXPECT_FALSE( obj.isPrimitive());
    EXPECT_FALSE( obj["vlist"].isPrimitive() );
    EXPECT_FALSE( obj["vmap"].isPrimitive() );

    EXPECT_TRUE( obj["vbool"].isPrimitive() );
    EXPECT_TRUE( obj["vint"].isPrimitive() );
    EXPECT_TRUE( obj["vdouble"].isPrimitive() );
    EXPECT_TRUE( obj["vstring"].isPrimitive() );

    EXPECT_TRUE( obj["vbool"].isBool() );
    EXPECT_TRUE( obj["vint"].isNumber() );
    EXPECT_TRUE( obj["vdouble"].isNumber() );
    EXPECT_TRUE( obj["vstring"].isString() );
}


TYPED_TEST( JsonioBaseTest, toType )
{
    auto& obj = *this->test_object;

    EXPECT_EQ( true, obj["vbool"].toBool() );
    EXPECT_EQ( false, obj["vdouble"].toBool() );
    EXPECT_EQ( false, obj["vmap"].toBool() );
    EXPECT_EQ( false, obj["vlist"].toBool() );

    EXPECT_EQ( -100, obj["vint"].toDouble() );
    EXPECT_EQ( 5.2, obj["vdouble"].toDouble() );
    EXPECT_EQ( 0.0, obj["vbool"].toDouble() );
    EXPECT_EQ( 0.0, obj["vlist"].toDouble() );

    EXPECT_EQ( -100, obj["vint"].toInt() );
    EXPECT_EQ( 0, obj["vdouble"].toInt() );
    obj["vdouble"] = 2.5;
    EXPECT_EQ( 0, obj["vdouble"].toInt() );
    EXPECT_EQ( 0, obj["vbool"].toInt() );
    EXPECT_EQ( 0, obj["vlist"].toInt() );

    EXPECT_EQ( "true", obj["vbool"].toString() );
    EXPECT_EQ( "-100", obj["vint"].toString() );
    EXPECT_EQ( "2.5", obj["vdouble"].toString() );
    EXPECT_EQ( "Test string", obj["vstring"].toString() );
    EXPECT_EQ( "[1.7,2.7,3.7,5.7]", obj["vlist"].toString(true) );
    EXPECT_EQ( "{\"key1\":\"val1\",\"key2\":\"val2\"}", obj["vmap"].toString(true) );
}


TYPED_TEST( JsonioBaseTest, getTypeSize )
{
    const auto& obj = *this->test_object;

    EXPECT_EQ( JsonBase::Bool, obj["vbool"].type() );
    EXPECT_EQ( JsonBase::String, obj["vstring"].type() );
    EXPECT_EQ( JsonBase::Int, obj["vint"].type() );
    EXPECT_EQ( JsonBase::Double, obj["vdouble"].type() );
    EXPECT_EQ( JsonBase::Array, obj["vlist"].type() );
    EXPECT_EQ( JsonBase::Object, obj["vmap"].type() );

    EXPECT_EQ( static_cast<size_t>(0), obj["vdouble"].size() );
    EXPECT_EQ( static_cast<size_t>(4), obj["vlist"].size() );
    EXPECT_EQ( static_cast<size_t>(2), obj["vmap"].size() );

    EXPECT_FALSE( obj.empty());
    EXPECT_FALSE( obj["vlist"].empty() );
    EXPECT_FALSE( obj["vmap"].empty() );
    EXPECT_TRUE( obj["vbool"].empty() );

    EXPECT_THROW( obj["vdouble"].child(0), jsonio_exception  );
    EXPECT_EQ( obj["vlist"][3].dump(), obj["vlist"].child(3).dump() );
    EXPECT_EQ( obj["vmap"]["key1"].dump(), obj["vmap"].child(0).dump() );
    EXPECT_EQ( obj["vdouble"].dump(), obj.child(2).dump() );

}

/*
TYPED_TEST( JsonioBaseTest, getQtGUI )
{
    const auto& obj = *this->test_object;

    EXPECT_EQ( "vbool", obj["vbool"].getKey() );
    EXPECT_EQ( "vdouble", obj["vdouble"].getKey() );
    EXPECT_EQ( "1", obj["vlist"][1].getKey() );
    EXPECT_EQ( "key1", obj["vmap"]["key1"].getKey() );

    EXPECT_EQ( "true", obj["vbool"].getFieldValue() );
    EXPECT_EQ( "5.2", obj["vdouble"].getFieldValue() );
    EXPECT_EQ( "2.7", obj["vlist"][1].getFieldValue() );
    EXPECT_EQ( "val1", obj["vmap"]["key1"].getFieldValue() );
    EXPECT_EQ( "", obj["vlist"].getFieldValue() );
    EXPECT_EQ( "", obj["vmap"].getFieldValue() );

    EXPECT_EQ( 5, obj["vmap"].getNdx() );
    EXPECT_EQ( 2, obj["vdouble"].getNdx() );
    EXPECT_EQ( 3, obj["vlist"][3].getNdx() );
    EXPECT_EQ( 0, obj["vmap"]["key1"].getNdx() );

    EXPECT_EQ( 0, obj["vdouble"].getChildrenCount() );
    EXPECT_EQ( 4, obj["vlist"].getChildrenCount() );
    EXPECT_EQ( 2, obj["vmap"].getChildrenCount() );
    EXPECT_EQ( 6, obj.getChildrenCount() );

    // getChild
    EXPECT_EQ( nullptr, obj["vdouble"].getChild(0) );
    EXPECT_EQ( &obj["vlist"][3], obj["vlist"].getChild(3) );
    EXPECT_EQ( &obj["vmap"]["key1"], obj["vmap"].getChild(0) );
    EXPECT_EQ( &obj["vdouble"], obj.getChild(2) );

    // getParent
    EXPECT_EQ( &obj, obj["vdouble"].getParent() );
    EXPECT_EQ( &obj["vlist"], obj["vlist"][1].getParent() );
    EXPECT_EQ( &obj["vmap"], obj["vmap"]["key1"].getParent() );
    EXPECT_EQ( nullptr, obj.getParent() );

    // getUsedKeys - different for
    std::vector<std::string> usekeys{"vbool","vint","vdouble","vstring","vlist","vmap"};
    EXPECT_EQ( usekeys, obj.getUsedKeys() );

    // getFieldPath
    EXPECT_EQ( "vdouble", obj["vdouble"].get_path() );
    EXPECT_EQ( "vlist.1", obj["vlist"][1].get_path() );
    EXPECT_EQ( "vmap.key1", obj["vmap"]["key1"].get_path() );
    EXPECT_EQ( "top", obj.get_path() );
}
*/

TYPED_TEST( JsonioBaseTest, get_to )
{
    const auto& obj = *this->test_object;

    bool vbool;
    EXPECT_TRUE( obj["vbool"].get_to( vbool ) );
    EXPECT_EQ( true, vbool );

    int  vint;
    EXPECT_TRUE( obj["vint"].get_to( vint ));
    EXPECT_EQ( -100, vint );

    double vdouble;
    EXPECT_TRUE( obj["vdouble"].get_to( vdouble ));
    EXPECT_EQ( 5.2, vdouble );

    std::string vstr;
    EXPECT_TRUE( obj["vstring"].get_to( vstr ));
    EXPECT_EQ( "Test string", vstr );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 3.7, 5.7 };
    EXPECT_TRUE( obj["vlist"].get_to(vlist2) );
    EXPECT_EQ( vlist, vlist2 );

    std::unordered_map<std::string, std::string> vumaps2, vumaps = { {"key1", "val1" }, {"key2", "val2" } };
    EXPECT_TRUE( obj["vmap"].get_to(vumaps2) );
    EXPECT_EQ( vumaps, vumaps2 );
}

TYPED_TEST( JsonioBaseTest, set_from )
{
    auto& obj = *this->test_object;

    bool vbool{false};
    obj["vbool"].set_from( vbool );
    int  vint{15};
    obj["vint"].set_from( vint );
    double vdouble{-2.5};
    obj["vdouble"].set_from( vdouble );
    std::string vstr{"New string"};
    obj["vstring"].set_from( vstr );
    std::list<double> vlist{ 17, 27 };
    obj["vlist"].set_from(vlist);
    std::map<std::string, std::string> vumaps{ {"newkey1", "val11" }, {"newkey2", "val22" } };
    obj["vmap"].set_from(vumaps);

    EXPECT_EQ( "{\"vbool\":false,\"vint\":15,\"vdouble\":-2.5,\"vstring\":\"New string\",\"vlist\":[17,27],\"vmap\":{\"newkey1\":\"val11\",\"newkey2\":\"val22\"}}", obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, set_value )
{
    auto& obj = *this->test_object;

    obj["vbool"] = false;
    obj["vint"] = 15;
    obj["vdouble"] = -2.5;
    obj["vstring"] = "New string";
    obj["vlist"] = std::list<double>{ 17, 27 };
    obj["vmap"] = std::map<std::string, std::string>{ {"newkey1", "val11" }, {"newkey2", "val22" } };

    EXPECT_EQ( "{\"vbool\":false,\"vint\":15,\"vdouble\":-2.5,\"vstring\":\"New string\",\"vlist\":[17,27],\"vmap\":{\"newkey1\":\"val11\",\"newkey2\":\"val22\"}}", obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, set_null_value )
{
    auto& obj = *this->test_object;

    obj["vbool"].set_null();
    obj["vint"].set_null();
    obj["vdouble"].set_null();
    obj["vstring"].set_null();
    obj["vlist"].set_null();
    obj["vmap"].set_null();

    EXPECT_TRUE( obj["vbool"].isNull() );
    EXPECT_TRUE( obj["vint"].isNull() );
    EXPECT_TRUE( obj["vdouble"].isNull() );
    EXPECT_TRUE( obj["vstring"].isNull() );
    EXPECT_TRUE( obj["vlist"].isNull() );
    EXPECT_TRUE( obj["vmap"].isNull() );

    EXPECT_EQ( "{\"vbool\":null,\"vint\":null,\"vdouble\":null,\"vstring\":null,\"vlist\":null,\"vmap\":null}", obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, loads_null_value )
{
    auto& obj = *this->test_object;
    std::string null_json = "{\"vbool\":false,\"vint\":null,\"vdouble\":null,\"vstring\":\"New string\",\"vlist\":[17,null],\"vmap\":{\"newkey1\":\"val11\",\"newkey2\":null}}";
    obj.loads(null_json);
    EXPECT_TRUE( obj["vint"].isNull() );
    EXPECT_TRUE( obj["vdouble"].isNull() );
    EXPECT_FALSE( obj["vstring"].isNull() );
    EXPECT_TRUE( obj["vlist"][1].isNull() );
    EXPECT_TRUE( obj["vmap"]["newkey2"].isNull() );
    EXPECT_EQ( null_json, obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, set_after_null_value )
{
    auto& obj = *this->test_object;
    std::string null_json = "{\"vbool\":null,\"vint\":null,\"vdouble\":null,\"vstring\":null,\"vlist\":null,\"vmap\":null}";
    obj.loads(null_json);

    obj["vbool"] = false;
    obj["vint"] = 15;
    obj["vdouble"] = -2.5;
    obj["vstring"] = "New string";
    obj["vlist"] = std::list<double>{ 17, 27 };
    obj["vmap"] = std::map<std::string, std::string>{ {"newkey1", "val11" }, {"newkey2", "val22" } };

    EXPECT_EQ( "{\"vbool\":false,\"vint\":15,\"vdouble\":-2.5,\"vstring\":\"New string\",\"vlist\":[17,27],\"vmap\":{\"newkey1\":\"val11\",\"newkey2\":\"val22\"}}", obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, loads_empty_value )
{
    auto& obj = *this->test_object;
    std::string empty_json = "{\"vbool\":false,\"vint\":0,\"vdouble\":0,\"vstring\":\"\",\"vlist\":[],\"vmap\":{}}";
    obj.loads(empty_json);
    EXPECT_FALSE( obj["vint"].isNull() );
    EXPECT_FALSE( obj["vdouble"].isNull() );
    EXPECT_FALSE( obj["vstring"].isNull() );
    EXPECT_TRUE( obj["vlist"].isArray() );
    EXPECT_TRUE( obj["vmap"].isObject() );
    EXPECT_EQ( empty_json, obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, get_to_illegal_value )
{
    auto& obj = *this->test_object;

    bool vbool{true};
    EXPECT_TRUE( obj["vmap"].get_to( vbool ) );
    EXPECT_FALSE( vbool );

    int  vint{4};
    EXPECT_FALSE( obj["vmap"].get_to( vint ) );
    EXPECT_EQ( 4, vint );
    EXPECT_FALSE( obj["vbool"].get_to( vint ) );
    EXPECT_EQ( 0, vint );
    EXPECT_FALSE( obj["vdouble"].get_to( vint ) );
    EXPECT_EQ( 5, vint );

    double vdouble{0.1};
    EXPECT_FALSE( obj["vstring"].get_to( vdouble ));
    EXPECT_EQ( 0, vdouble );
    EXPECT_TRUE( obj["vint"].get_to( vdouble ));
    EXPECT_EQ( -100, vdouble );
    obj["vstring"] = "135";
    EXPECT_TRUE( obj["vstring"].get_to( vdouble ));
    EXPECT_EQ( 135, vdouble );

    std::string vstr{"test"};
    EXPECT_TRUE( obj["vdouble"].get_to( vstr ));
    EXPECT_EQ( vstr, "5.2" );
    EXPECT_TRUE( obj["vlist"].get_to( vstr ));
    EXPECT_EQ( vstr, "[1.7,2.7,3.7,5.7]" );

    std::list<std::string> vlist;
    EXPECT_FALSE( obj["vdouble"].get_to( vlist ) );
    EXPECT_TRUE( vlist.empty() );
    EXPECT_TRUE( obj["vmap"].get_to(vlist));
    EXPECT_FALSE( vlist.empty() );

    std::unordered_map<std::string, double> vumaps;
    EXPECT_FALSE( obj["vstring"].get_to( vumaps ) );
    EXPECT_TRUE( vumaps.empty() );
    EXPECT_TRUE( obj["vlist"].get_to(vumaps));
    EXPECT_FALSE( vumaps.empty() );
}

TYPED_TEST( JsonioBaseTest, ArrayResize )
{
    auto& obj = *this->test_object;

    auto sizes = obj["vlist"].array_sizes();
    EXPECT_EQ(static_cast<size_t>(1), sizes.size() );
    EXPECT_EQ( static_cast<size_t>(4), sizes[0] );

    obj["vlist"].array_resize(2, "5");
    EXPECT_EQ( obj["vlist"].dump(true), "[1.7,2.7]" );
    obj["vlist"].array_resize(6, "5");
    EXPECT_EQ( obj["vlist"].dump(true), "[1.7,2.7,5,5,5,5]" );
    obj["vlist"].array_resize(8, "");
    EXPECT_EQ( obj["vlist"].dump(true), "[1.7,2.7,5,5,5,5,1.7,1.7]" );
}

TYPED_TEST( JsonioBaseTest, Iterators )
{
    auto& obj = *this->test_object;

    // range-based for
    size_t ii=0;
    for (const auto& element : obj["vlist"])
    {
        EXPECT_EQ( obj["vlist"][ii++].toDouble(), element->toDouble() );
    }
    EXPECT_EQ( ii, obj["vlist"].size() );

    // range-based for
    int jj=0;
    for (auto it = obj["vdouble"].begin(); it != obj["vdouble"].end(); ++it, ++jj);
    EXPECT_EQ( jj, 0 );


    // iterate the array
    ii=0;
    for (auto it = obj.begin(); it != obj.end(); ++it, ++ii)
    {
        EXPECT_EQ( obj[ (*it)->getKey() ].toString(), (*it)->toString() );
    }
    EXPECT_EQ( ii, obj.size() );
}
