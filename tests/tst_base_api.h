#pragma once

#include <gtest/gtest.h>

#include <list>
#include <map>
#include <unordered_map>
#include "jsonio14/jsonfree.h"

using namespace testing;
using namespace jsonio14;


template <typename T>
class JsonioBaseTest : public ::testing::Test
{

public:

    const std::string schemaName = "SimpleSchemaTest";
    const std::string input_json = "{\"vbool\":true,\"vint\":-100,\"vdouble\":5.2,\"vstring\":\"Test string\","
                                   "\"vlist\":[1.7,2.7,3.7,5.7],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}";

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

using JsonTypes = ::testing::Types<JsonFree>;
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
    EXPECT_EQ( "[1.7,2.7,3.7,5.7]\n", obj["vlist"].toString(true) );
    EXPECT_EQ( "{\"key1\":\"val1\",\"key2\":\"val2\"}\n", obj["vmap"].toString(true) );
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

    EXPECT_EQ( 0, obj["vdouble"].size() );
    EXPECT_EQ( 4, obj["vlist"].size() );
    EXPECT_EQ( 2, obj["vmap"].size() );

    EXPECT_FALSE( obj.empty());
    EXPECT_FALSE( obj["vlist"].empty() );
    EXPECT_FALSE( obj["vmap"].empty() );
    EXPECT_TRUE( obj["vbool"].empty() );
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

    EXPECT_EQ( "{\"vbool\":false,\"vint\":15,\"vdouble\":-2.5,\"vstring\":\"New string\",\"vlist\":[17,27],\"vmap\":{\"newkey1\":\"val11\",\"newkey2\":\"val22\"}}\n", obj.toString(true) );
}

TYPED_TEST( JsonioBaseTest, get_to_no_exist )
{
    auto& obj = *this->test_object;

    bool vbool{true};
    EXPECT_TRUE( obj["noexist1"].get_to( vbool ) );
    EXPECT_FALSE( vbool );

    int  vint{5};
    EXPECT_FALSE( obj["noexist2"].get_to( vint ) );
    EXPECT_EQ( 5, vint );

    double vdouble{0.1};
    EXPECT_FALSE( obj["noexist3"].get_to( vdouble ));
    EXPECT_EQ( 0.1, vdouble );

    std::string vstr{"test"};
    EXPECT_TRUE( obj["noexist4"].get_to( vstr ));
    EXPECT_TRUE( vstr.empty() );

    std::list<double> vlist{ 1.7, 5.7 };
    EXPECT_TRUE( obj["noexist5"].get_to(vlist));
    EXPECT_TRUE( vlist.empty() );

    EXPECT_TRUE( obj["noexist8"]["list"].get_to(vlist));
    EXPECT_TRUE( vlist.empty() );

    std::unordered_map<std::string, std::string> vumaps{ {"key1", "val1" }, {"key2", "val2" } };
    EXPECT_TRUE( obj["noexist6"].get_to(vumaps));
    EXPECT_TRUE( vumaps.empty() );

    EXPECT_TRUE( obj["noexist9"]["map"].get_to(vumaps));
    EXPECT_TRUE( vumaps.empty() );

    const auto& constobj = *this->test_object;
    EXPECT_THROW( constobj["noexist7"].get_to( vint ), jarango_exception );

    //std::cout << "Test get_to_no_exist" << obj << std::endl;
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
    EXPECT_EQ( vstr, "[1.7,2.7,3.7,5.7]\n" );

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
    EXPECT_EQ( 1, sizes.size() );
    EXPECT_EQ( 4, sizes[0] );

    obj["vlist"].array_resize(2, "5");
    EXPECT_EQ( obj["vlist"].dump(true), "[1.7,2.7]\n" );
    obj["vlist"].array_resize(6, "5");
    EXPECT_EQ( obj["vlist"].dump(true), "[1.7,2.7,5,5,5,5]\n" );
    obj["vlist"].array_resize(8, "");
    EXPECT_EQ( obj["vlist"].dump(true), "[1.7,2.7,5,5,5,5,1.7,1.7]\n" );
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
    ii=0;
    for (auto it = obj["vdouble"].begin(); it != obj["vdouble"].end(); ++it, ++ii);
    EXPECT_EQ( ii, 0 );


    // iterate the array
    ii=0;
    for (auto it = obj.begin(); it != obj.end(); ++it)
    {
        EXPECT_EQ( obj[ii++].toString(), (*it)->toString() );
    }
    EXPECT_EQ( ii, obj.size() );
}
