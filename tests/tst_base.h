#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsonfree.h"
#include "jsondump.h"

using namespace testing;
using namespace jsonio14;

TEST( JsonioBase, typeTraits )
{
    bool vbool = true;
    char vchar = 'a';
    int  vint  = -1;
    long vlong = 2l;
    unsigned int  vuint = 3;
    long long vlonglong = 4L;
    float vfloat = 2.5;
    double vdouble = 1e-10;
    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    std::set<int> vset = { 1, 2, 6, 4 };
    std::list<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::vector<int> vvec = { 1, 2, 3, 4};
    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };

    EXPECT_EQ( JsonBase::Bool, JsonBase::typeTraits( vbool ) );
    EXPECT_EQ( JsonBase::String, JsonBase::typeTraits( vchar ) );
    EXPECT_EQ( JsonBase::Int, JsonBase::typeTraits( vint ) );
    EXPECT_EQ( JsonBase::Int, JsonBase::typeTraits( vlong ) );
    EXPECT_EQ( JsonBase::Int, JsonBase::typeTraits( vuint ) );
    EXPECT_EQ( JsonBase::Int, JsonBase::typeTraits( vlonglong ) );
    EXPECT_EQ( JsonBase::Double, JsonBase::typeTraits( vfloat ) );
    EXPECT_EQ( JsonBase::Double, JsonBase::typeTraits( vdouble ) );
    EXPECT_EQ( JsonBase::Array, JsonBase::typeTraits( vvecs ) );
    EXPECT_EQ( JsonBase::Array, JsonBase::typeTraits( vset ) );
    EXPECT_EQ( JsonBase::Array, JsonBase::typeTraits( vlist ) );
    EXPECT_EQ( JsonBase::Object, JsonBase::typeTraits( vmap ) );
    EXPECT_EQ( JsonBase::Object, JsonBase::typeTraits( vumaps ) );
    EXPECT_EQ( JsonBase::UNDEFINED, JsonBase::typeTraits( nullptr ) );
    EXPECT_EQ( JsonBase::String, JsonBase::typeTraits( std::string("TestString") ) );
    EXPECT_EQ( JsonBase::String, JsonBase::typeTraits( "Test char *" ) );
}

TEST( JsonioBase, set_list_from )
{
    auto jsFree = JsonFree::object();

    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    jsFree.set_list_from(vvecs);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[\"v1\",\"v2\",\"v3\",\"v4\"]\n", json::dump( jsFree, true ) );

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.set_list_from(vset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,6]\n", json::dump( jsFree, true ) );

    std::list<double> vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.set_list_from(vlist);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1.7,2.7,4.7,3.7,0.5]\n", json::dump( jsFree, true ) );

    std::vector<int> vvec = { 1, 2, 4, 3};
    jsFree.set_list_from(vvec);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,3]\n", json::dump( jsFree, true ) );

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.set_list_from(fwd_list);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[11,12,13,14,15]\n", json::dump( jsFree, true ) );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.set_list_from(uset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.set_list_from(mset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1,3,4,5,5]\n", json::dump( jsFree, true ) );
}


TEST( JsonioBase, set_map_from )
{
    auto jsFree = JsonFree::object();

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.set_map_from(vmap);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key1\":4,\"key2\":5}\n", json::dump( jsFree, true ) );

    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.set_map_from(vumaps);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key2\":\"val5\",\"key1\":\"val4\"}\n", json::dump( jsFree, true ) );

    //std::multimap<std::string, int> vmmap = { {"key1", 4 }, {"key2", 5 }/*, {"key1", 8 }*/ };
    // not used  jsFree.set_map_from(vmmap);
}

TEST( JsonioBase, set_from )
{
    auto jsFree = JsonFree::object();

    bool vbool = true;
    jsFree["vbool"].set_from( vbool );
    EXPECT_TRUE( jsFree["vbool"].isBool() );
    EXPECT_EQ( jsFree["vbool"].toBool(), vbool );

    char vchar = 'a';
    jsFree["vchar"].set_from( vchar );
    EXPECT_TRUE( jsFree["vchar"].isString() );
    EXPECT_EQ( jsFree["vchar"].toString(), "a" );

    int  vint  = -1;
    jsFree["vint"].set_from( vint );
    EXPECT_TRUE( jsFree["vint"].isNumber() );
    EXPECT_EQ( jsFree["vint"].toInt(), vint );

    long vlong = 2l;
    jsFree["vlong"].set_from( vlong );
    EXPECT_TRUE( jsFree["vlong"].isNumber() );
    EXPECT_EQ( jsFree["vlong"].toInt(), vlong );

    unsigned int  vuint = 3;
    jsFree["vuint"].set_from( vuint );
    EXPECT_TRUE( jsFree["vuint"].isNumber() );
    EXPECT_EQ( jsFree["vuint"].toInt(), vuint );

    long long vlonglong = 4L;
    jsFree["vlonglong"].set_from( vlonglong );
    EXPECT_TRUE( jsFree["vlonglong"].isNumber() );
    EXPECT_EQ( jsFree["vlonglong"].toInt(), vlonglong );

    float vfloat = 2.5;
    jsFree["vfloat"].set_from( vfloat );
    EXPECT_TRUE( jsFree["vfloat"].isNumber() );
    EXPECT_EQ( jsFree["vfloat"].toDouble(), vfloat );

    double vdouble = 1e-10;
    jsFree["vdouble"].set_from( vdouble );
    EXPECT_TRUE( jsFree["vdouble"].isNumber() );
    EXPECT_EQ( jsFree["vdouble"].toDouble(), vdouble );

    // json::dump(std::cout, jsFree, true);
}

TEST( JsonioBase, set_fromArray )
{
    auto jsFree = JsonFree::object();

    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    jsFree.set_from(vvecs);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[\"v1\",\"v2\",\"v3\",\"v4\"]\n", json::dump( jsFree, true ) );

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.set_from(vset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,6]\n", json::dump( jsFree, true ) );

    std::list<double> vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.set_from(vlist);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1.7,2.7,4.7,3.7,0.5]\n", json::dump( jsFree, true ) );

    std::vector<int> vvec = { 1, 2, 4, 3};
    jsFree.set_from(vvec);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,3]\n", json::dump( jsFree, true ) );

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.set_from(fwd_list);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[11,12,13,14,15]\n", json::dump( jsFree, true ) );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.set_from(uset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.set_from(mset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1,3,4,5,5]\n", json::dump( jsFree, true ) );
}

TEST( JsonioBase, set_fromMapKey )
{
    auto jsFree = JsonFree::object();

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.set_from(vmap);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key1\":4,\"key2\":5}\n", json::dump( jsFree, true ) );

    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.set_from(vumaps);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key2\":\"val5\",\"key1\":\"val4\"}\n", json::dump( jsFree, true ) );
}

TEST( JsonioBase, get_to_list )
{
    std::vector<std::string> vvecs2, vvecs = { "v1", "v2", "v3", "v4" };
    auto jsFree = json::loads("[\"v1\",\"v2\",\"v3\",\"v4\"]\n");
    jsFree.get_to_list(vvecs2);
    EXPECT_EQ( vvecs, vvecs2 );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    json::loads("[1.7,2.7,4.7,3.7,0.5]\n", jsFree);
    jsFree.get_to_list(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::vector<int> vvec2, vvec = { 1, 2, 4, 3};
    json::loads("[1,2,4,3]\n", jsFree);
    jsFree.get_to_list(vvec2);
    EXPECT_EQ( vvec, vvec2 );

    //  std::forward_list<int> fwd_list= {11,12,13,14,15}; no emplace_back for
    // std::set<int> vset2, vset = { 1, 2, 6, 4 }; no emplace_back for set
    // std::unordered_set<int> uset = {1, 3, 5, 4, 5}; no emplace_back for
    // std::multiset<int> mset = {1, 3, 5, 4, 5}; no emplace_back for
}

TEST( JsonioBase, getMapKey )
{
    std::map<std::string, int> vmap2, vmap = { {"key1", 4 }, {"key2", 5 } };
    auto jsFree = json::loads("{\"key1\":4,\"key2\":5}\n");
    jsFree.get_to_map(vmap2);
    EXPECT_EQ( vmap, vmap2 );

    std::unordered_map<std::string, std::string> vumaps2, vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree = json::loads("{\"key2\":\"val5\",\"key1\":\"val4\"}\n");
    jsFree.get_to_map(vumaps2);
    EXPECT_EQ( vumaps, vumaps2 );
}


TEST( JsonioBase, get_to )
{
    auto jsFree = json::loads("{\"vbool\":true,\"vchar\":\"a\",\"vint\":-1,\"vlong\":2,\"vuint\":3,\"vlonglong\":4,\"vfloat\":2.5,\"vdouble\":1e-10}");

    bool vbool;
    jsFree["vbool"].get_to( vbool );
    EXPECT_EQ( true, vbool );

    char vchar;
    jsFree["vchar"].get_to( vchar );
    EXPECT_EQ( 'a', vchar );

    int  vint;
    jsFree["vint"].get_to( vint );
    EXPECT_EQ( -1, vint );

    long vlong;
    jsFree["vlong"].get_to( vlong );
    EXPECT_EQ( 2l, vlong );

    unsigned int  vuint;
    jsFree["vuint"].get_to( vuint );
    EXPECT_EQ( 3, vuint );

    long long vlonglong;
    jsFree["vlonglong"].get_to( vlonglong );
    EXPECT_EQ( 4L, vlonglong );

    float vfloat;
    jsFree["vfloat"].get_to( vfloat );
    EXPECT_EQ( 2.5, vfloat );

    double vdouble;
    jsFree["vdouble"].get_to( vdouble );
    EXPECT_EQ( 1e-10, vdouble );
}

TEST( JsonioBase, get_toArray )
{
    std::vector<std::string> vvecs2, vvecs = { "v1", "v2", "v3", "v4" };
    auto jsFree = json::loads("[\"v1\",\"v2\",\"v3\",\"v4\"]\n");
    jsFree.get_to(vvecs2);
    EXPECT_EQ( vvecs, vvecs2 );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    json::loads("[1.7,2.7,4.7,3.7,0.5]\n", jsFree);
    jsFree.get_to(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::vector<int> vvec2, vvec = { 1, 2, 4, 3};
    json::loads("[1,2,4,3]\n", jsFree);
    jsFree.get_to(vvec2);
    EXPECT_EQ( vvec, vvec2 );

    // std::forward_list<int> fwd_list= {11,12,13,14,15}; no emplace_back for
    // std::set<int> vset2, vset = { 1, 2, 6, 4 }; no emplace_back for set
    // std::unordered_set<int> uset = {1, 3, 5, 4, 5}; no emplace_back for
    // std::multiset<int> mset = {1, 3, 5, 4, 5}; no emplace_back for
}

TEST( JsonioBase, get_toMapKey )
{
    std::map<std::string, int> vmap2, vmap = { {"key1", 4 }, {"key2", 5 } };
    auto jsFree = json::loads("{\"key1\":4,\"key2\":5}\n");
    jsFree.get_to(vmap2);
    EXPECT_EQ( vmap, vmap2 );

    std::unordered_map<std::string, std::string> vumaps2, vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree = json::loads("{\"key2\":\"val5\",\"key1\":\"val4\"}\n");
    jsFree.get_to(vumaps2);
    EXPECT_EQ( vumaps, vumaps2 );
}


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
    json::loads( input_json, *test_object );
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
    EXPECT_EQ( 5, obj["vdouble"].toInt() );
    obj["vdouble"] = 2.5;
    EXPECT_EQ( 2, obj["vdouble"].toInt() );
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
    EXPECT_EQ( "vdouble", obj["vdouble"].getFieldPath() );
    EXPECT_EQ( "vlist.1", obj["vlist"][1].getFieldPath() );
    EXPECT_EQ( "vmap.key1", obj["vmap"]["key1"].getFieldPath() );
    EXPECT_EQ( "top", obj.getFieldPath() );
}

TYPED_TEST( JsonioBaseTest, get_to )
{
    const auto& obj = *this->test_object;

    bool vbool;
    obj["vbool"].get_to( vbool );
    EXPECT_EQ( true, vbool );

    int  vint;
    obj["vint"].get_to( vint );
    EXPECT_EQ( -100, vint );

    double vdouble;
    obj["vdouble"].get_to( vdouble );
    EXPECT_EQ( 5.2, vdouble );

    std::string vstr;
    obj["vstring"].get_to( vstr );
    EXPECT_EQ( "Test string", vstr );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 3.7, 5.7 };
    obj["vlist"].get_to(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::unordered_map<std::string, std::string> vumaps2, vumaps = { {"key1", "val1" }, {"key2", "val2" } };
    obj["vmap"].get_to(vumaps2);
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
