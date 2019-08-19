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

TEST( JsonioBase, setArray )
{
    auto jsFree = JsonFree::object();

    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    jsFree.setArray(vvecs);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[\"v1\",\"v2\",\"v3\",\"v4\"]\n", json::dump( jsFree, true ) );

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.setArray(vset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,6]\n", json::dump( jsFree, true ) );

    std::list<double> vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.setArray(vlist);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1.7,2.7,4.7,3.7,0.5]\n", json::dump( jsFree, true ) );

    std::vector<int> vvec = { 1, 2, 4, 3};
    jsFree.setArray(vvec);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,3]\n", json::dump( jsFree, true ) );

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.setArray(fwd_list);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[11,12,13,14,15]\n", json::dump( jsFree, true ) );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.setArray(uset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.setArray(mset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1,3,4,5,5]\n", json::dump( jsFree, true ) );
}


TEST( JsonioBase, setMapKey )
{
    auto jsFree = JsonFree::object();

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.setMapKey(vmap);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key1\":4,\"key2\":5}\n", json::dump( jsFree, true ) );

    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.setMapKey(vumaps);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key2\":\"val5\",\"key1\":\"val4\"}\n", json::dump( jsFree, true ) );

    //std::multimap<std::string, int> vmmap = { {"key1", 4 }, {"key2", 5 }/*, {"key1", 8 }*/ };
    // not used  jsFree.setMapKey(vmmap);
}

TEST( JsonioBase, setValue )
{
    auto jsFree = JsonFree::object();

    bool vbool = true;
    jsFree["vbool"].setValue( vbool );
    EXPECT_TRUE( jsFree["vbool"].isBool() );
    EXPECT_EQ( jsFree["vbool"].toBool(), vbool );

    char vchar = 'a';
    jsFree["vchar"].setValue( vchar );
    EXPECT_TRUE( jsFree["vchar"].isString() );
    EXPECT_EQ( jsFree["vchar"].toString(), "a" );

    int  vint  = -1;
    jsFree["vint"].setValue( vint );
    EXPECT_TRUE( jsFree["vint"].isNumber() );
    EXPECT_EQ( jsFree["vint"].toInt(), vint );

    long vlong = 2l;
    jsFree["vlong"].setValue( vlong );
    EXPECT_TRUE( jsFree["vlong"].isNumber() );
    EXPECT_EQ( jsFree["vlong"].toInt(), vlong );

    unsigned int  vuint = 3;
    jsFree["vuint"].setValue( vuint );
    EXPECT_TRUE( jsFree["vuint"].isNumber() );
    EXPECT_EQ( jsFree["vuint"].toInt(), vuint );

    long long vlonglong = 4L;
    jsFree["vlonglong"].setValue( vlonglong );
    EXPECT_TRUE( jsFree["vlonglong"].isNumber() );
    EXPECT_EQ( jsFree["vlonglong"].toInt(), vlonglong );

    float vfloat = 2.5;
    jsFree["vfloat"].setValue( vfloat );
    EXPECT_TRUE( jsFree["vfloat"].isNumber() );
    EXPECT_EQ( jsFree["vfloat"].toDouble(), vfloat );

    double vdouble = 1e-10;
    jsFree["vdouble"].setValue( vdouble );
    EXPECT_TRUE( jsFree["vdouble"].isNumber() );
    EXPECT_EQ( jsFree["vdouble"].toDouble(), vdouble );

    // json::dump(std::cout, jsFree, true);
}

TEST( JsonioBase, setValueArray )
{
    auto jsFree = JsonFree::object();

    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    jsFree.setValue(vvecs);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[\"v1\",\"v2\",\"v3\",\"v4\"]\n", json::dump( jsFree, true ) );

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.setValue(vset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,6]\n", json::dump( jsFree, true ) );

    std::list<double> vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.setValue(vlist);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1.7,2.7,4.7,3.7,0.5]\n", json::dump( jsFree, true ) );

    std::vector<int> vvec = { 1, 2, 4, 3};
    jsFree.setValue(vvec);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,3]\n", json::dump( jsFree, true ) );

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.setValue(fwd_list);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[11,12,13,14,15]\n", json::dump( jsFree, true ) );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.setValue(uset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.setValue(mset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1,3,4,5,5]\n", json::dump( jsFree, true ) );
}

TEST( JsonioBase, setValueMapKey )
{
    auto jsFree = JsonFree::object();

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.setValue(vmap);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key1\":4,\"key2\":5}\n", json::dump( jsFree, true ) );

    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.setValue(vumaps);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key2\":\"val5\",\"key1\":\"val4\"}\n", json::dump( jsFree, true ) );
}

TEST( JsonioBase, getArray )
{
    std::vector<std::string> vvecs2, vvecs = { "v1", "v2", "v3", "v4" };
    auto jsFree = json::loads("[\"v1\",\"v2\",\"v3\",\"v4\"]\n");
    jsFree.getArray(vvecs2);
    EXPECT_EQ( vvecs, vvecs2 );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    json::loads("[1.7,2.7,4.7,3.7,0.5]\n", jsFree);
    jsFree.getArray(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::vector<int> vvec2, vvec = { 1, 2, 4, 3};
    json::loads("[1,2,4,3]\n", jsFree);
    jsFree.getArray(vvec2);
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
    jsFree.getMapKey(vmap2);
    EXPECT_EQ( vmap, vmap2 );

    std::unordered_map<std::string, std::string> vumaps2, vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree = json::loads("{\"key2\":\"val5\",\"key1\":\"val4\"}\n");
    jsFree.getMapKey(vumaps2);
    EXPECT_EQ( vumaps, vumaps2 );
}


TEST( JsonioBase, getValue )
{
    auto jsFree = json::loads("{\"vbool\":true,\"vchar\":\"a\",\"vint\":-1,\"vlong\":2,\"vuint\":3,\"vlonglong\":4,\"vfloat\":2.5,\"vdouble\":1e-10}");

    bool vbool;
    jsFree["vbool"].getValue( vbool );
    EXPECT_EQ( true, vbool );

    char vchar;
    jsFree["vchar"].getValue( vchar );
    EXPECT_EQ( 'a', vchar );

    int  vint;
    jsFree["vint"].getValue( vint );
    EXPECT_EQ( -1, vint );

    long vlong;
    jsFree["vlong"].getValue( vlong );
    EXPECT_EQ( 2l, vlong );

    unsigned int  vuint;
    jsFree["vuint"].getValue( vuint );
    EXPECT_EQ( 3, vuint );

    long long vlonglong;
    jsFree["vlonglong"].getValue( vlonglong );
    EXPECT_EQ( 4L, vlonglong );

    float vfloat;
    jsFree["vfloat"].getValue( vfloat );
    EXPECT_EQ( 2.5, vfloat );

    double vdouble;
    jsFree["vdouble"].getValue( vdouble );
    EXPECT_EQ( 1e-10, vdouble );
}

TEST( JsonioBase, getValueArray )
{
    std::vector<std::string> vvecs2, vvecs = { "v1", "v2", "v3", "v4" };
    auto jsFree = json::loads("[\"v1\",\"v2\",\"v3\",\"v4\"]\n");
    jsFree.getValue(vvecs2);
    EXPECT_EQ( vvecs, vvecs2 );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    json::loads("[1.7,2.7,4.7,3.7,0.5]\n", jsFree);
    jsFree.getValue(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::vector<int> vvec2, vvec = { 1, 2, 4, 3};
    json::loads("[1,2,4,3]\n", jsFree);
    jsFree.getValue(vvec2);
    EXPECT_EQ( vvec, vvec2 );

    // std::forward_list<int> fwd_list= {11,12,13,14,15}; no emplace_back for
    // std::set<int> vset2, vset = { 1, 2, 6, 4 }; no emplace_back for set
    // std::unordered_set<int> uset = {1, 3, 5, 4, 5}; no emplace_back for
    // std::multiset<int> mset = {1, 3, 5, 4, 5}; no emplace_back for
}

TEST( JsonioBase, getValueMapKey )
{
    std::map<std::string, int> vmap2, vmap = { {"key1", 4 }, {"key2", 5 } };
    auto jsFree = json::loads("{\"key1\":4,\"key2\":5}\n");
    jsFree.getValue(vmap2);
    EXPECT_EQ( vmap, vmap2 );

    std::unordered_map<std::string, std::string> vumaps2, vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree = json::loads("{\"key2\":\"val5\",\"key1\":\"val4\"}\n");
    jsFree.getValue(vumaps2);
    EXPECT_EQ( vumaps, vumaps2 );
}


template <typename T>
class JsonioBaseTest : public ::testing::Test
{

public:

    const std::string schemaName = "SimpleSchemaTest";
    const std::string input_json = "{\"vbool\":true,\"vint\":-100,\"vdouble\":1e-10,\"vstring\":\"Test string\","
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

TYPED_TEST(JsonioBaseTest, isTop )
{
    const auto& obj = *this->test_object;
    EXPECT_TRUE( obj.isTop());
    EXPECT_FALSE( obj["vlist"].isTop() );
}




