#pragma once

#include <gtest/gtest.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsonio17/jsonfree.h"
#include "jsonio17/jsondump.h"
using namespace testing;
using namespace jsonio17;

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
    EXPECT_EQ( JsonBase::String, JsonBase::typeTraits( std::string("TestString") ) );
    EXPECT_EQ( JsonBase::String, JsonBase::typeTraits( "Test char *" ) );
    //EXPECT_EQ( JsonBase::UNDEFINED, JsonBase::typeTraits( nullptr ) );
    EXPECT_EQ( JsonBase::Null, JsonBase::typeTraits( nullptr ) );
}

TEST( JsonioBase, set_list_from )
{
    auto jsFree = JsonFree::object();

    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    jsFree.set_list_from(vvecs);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[\"v1\",\"v2\",\"v3\",\"v4\"]\n", jsFree.dump(  true ) );

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.set_list_from(vset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,6]\n", jsFree.dump(  true ) );

    std::list<double> vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.set_list_from(vlist);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1.7,2.7,4.7,3.7,0.5]\n", jsFree.dump(  true ) );

    std::vector<int> vvec = { 1, 2, 4, 3};
    jsFree.set_list_from(vvec);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,3]\n", jsFree.dump(  true ) );

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.set_list_from(fwd_list);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[11,12,13,14,15]\n", jsFree.dump(  true ) );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.set_list_from(uset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.set_list_from(mset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1,3,4,5,5]\n", jsFree.dump(  true ) );
}


TEST( JsonioBase, set_map_from )
{
    auto jsFree = JsonFree::object();

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.set_map_from(vmap);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key1\":4,\"key2\":5}\n", jsFree.dump(  true ) );

    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.set_map_from(vumaps);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key2\":\"val5\",\"key1\":\"val4\"}\n", jsFree.dump(  true ) );

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
    EXPECT_EQ( "[\"v1\",\"v2\",\"v3\",\"v4\"]\n", jsFree.dump(  true ) );

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.set_from(vset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,6]\n", jsFree.dump(  true ) );

    std::list<double> vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.set_from(vlist);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1.7,2.7,4.7,3.7,0.5]\n", jsFree.dump(  true ) );

    std::vector<int> vvec = { 1, 2, 4, 3};
    jsFree.set_from(vvec);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );
    EXPECT_EQ( "[1,2,4,3]\n", jsFree.dump(  true ) );

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.set_from(fwd_list);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[11,12,13,14,15]\n", jsFree.dump(  true ) );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.set_from(uset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 4 );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.set_from(mset);
    EXPECT_TRUE( jsFree.isArray() );
    EXPECT_EQ( jsFree.size(), 5 );
    EXPECT_EQ( "[1,3,4,5,5]\n", jsFree.dump(  true ) );
}

TEST( JsonioBase, set_fromMapKey )
{
    auto jsFree = JsonFree::object();

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.set_from(vmap);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key1\":4,\"key2\":5}\n", jsFree.dump(  true ) );

    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.set_from(vumaps);
    EXPECT_TRUE( jsFree.isObject() );
    EXPECT_EQ( jsFree.size(), 2 );
    EXPECT_EQ( "{\"key2\":\"val5\",\"key1\":\"val4\"}\n", jsFree.dump(  true ) );
}

TEST( JsonioBase, get_to_list )
{
    std::vector<std::string> vvecs2, vvecs = { "v1", "v2", "v3", "v4" };
    auto jsFree = json::loads("[\"v1\",\"v2\",\"v3\",\"v4\"]\n");
    jsFree.get_to_list(vvecs2);
    EXPECT_EQ( vvecs, vvecs2 );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.loads("[1.7,2.7,4.7,3.7,0.5]\n");
    jsFree.get_to_list(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::vector<int> vvec2, vvec = { 1, 2, 4, 3};
    jsFree.loads("[1,2,4,3]\n");
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
    jsFree.loads("[1.7,2.7,4.7,3.7,0.5]\n");
    jsFree.get_to(vlist2);
    EXPECT_EQ( vlist, vlist2 );

    std::vector<int> vvec2, vvec = { 1, 2, 4, 3};
    jsFree.loads("[1,2,4,3]\n");
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

TEST( JsonioBase, checked_value )
{
    EXPECT_EQ( "string", JsonBase::checked_value( JsonBase::String, "string" ) );
    EXPECT_EQ( "false", JsonBase::checked_value( JsonBase::Bool, "string" ) );
    EXPECT_EQ( "true", JsonBase::checked_value( JsonBase::Bool, "true" ) );
    EXPECT_EQ( "0", JsonBase::checked_value( JsonBase::Int, "string" ) );
    EXPECT_EQ( "10", JsonBase::checked_value( JsonBase::Int, "10" ) );
    EXPECT_EQ( "0", JsonBase::checked_value( JsonBase::Int, "1.5" ) );
    EXPECT_EQ( "0.0", JsonBase::checked_value( JsonBase::Double, "2string" ) );
    EXPECT_EQ( "10", JsonBase::checked_value( JsonBase::Double, "10" ) );
    EXPECT_EQ( "1.5", JsonBase::checked_value( JsonBase::Double, "1.5" ) );

    EXPECT_EQ( "", JsonBase::checked_value( JsonBase::Object, "string" ) );
    EXPECT_EQ( "", JsonBase::checked_value( JsonBase::Array, "string" ) );

    auto obj = json::loads("{\"key1\":4,\"key2\":5}\n");
    obj["vsetnew"] = JsonFree::array("vsetnew");
    EXPECT_EQ( obj["vsetnew"].dump(true), "[]\n" );
    obj["vsetnew"].array_resize(3, "4.1");
    EXPECT_EQ( obj["vsetnew"].dump(true), "[4.1,4.1,4.1]\n" );

}


//-------------------------------------------------------------------------------------

TEST( JsonioBase, ObjectAssignment)
{
    auto obj = json::loads("{\"about\":{\"version\":1,\"description\":\"About\"},\"formats\":"
                           "{\"int\":{\"width\":5,\"precision\":0}}}");

    auto obj2 = json::loads("{\"width\":20,\"precision\":10}");
    obj["formats"]["obj2"] = obj2;
    EXPECT_EQ( obj["formats"].size(), 2 );
    EXPECT_EQ( obj["formats"]["obj2"].dump(true), "{\"width\":20,\"precision\":10}\n" );

    obj["formats"]["obj3"] = json::loads("{\"width\":30,\"precision\":15}");
    EXPECT_EQ( obj["formats"].size(), 3 );
    EXPECT_EQ( obj["formats"]["obj3"].dump(true), "{\"width\":30,\"precision\":15}\n" );

}

TEST( JsonioBase, get_to_no_exist_free )
{
    auto obj = json::loads("{\"vbool\":true,\"vint\":-100,\"vdouble\":5.2,\"vstring\":\"Test string\","
                           "\"vlist\":[1.7,2.7,3.7,5.7],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}");

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

    const auto& constobj = obj;
    EXPECT_THROW( constobj["noexist7"].get_to( vint ), jsonio_exception );

    //std::cout << "Test get_to_no_exist" << obj << std::endl;
}

TEST( JsonioBase, AddValueViaPath )
{
    auto obj = json::loads("{\"about\":{\"version\":1,\"description\":\"About\"},\"formats\":"
                           "{\"int\":{\"width\":5,\"precision\":0},\"float\":{\"width\":10,\"precision\":4},"
                           "\"double\":{\"width\":15,\"precision\":6}},\"data\":[{\"group\":\"float\",\"value\":1.4},"
                           "{\"group\":\"int\",\"value\":100},{\"group\":\"double\",\"value\":1e-10},{\"group\":\"double\",\"value\":10000000000}],"
                           "\"values\":[[1,2,3],[11,12,13]]}");

    JsonFree& new_obj1 = obj.add_object_via_path("formats.add_object1");
    EXPECT_TRUE( new_obj1.isObject() );
    EXPECT_TRUE( new_obj1.empty() );
    EXPECT_TRUE( obj.path_if_exists( "formats.add_object1" ) );

    EXPECT_THROW( obj.add_object_via_path("data[10]"), jsonio_exception );

    int iwidth = 10;
    EXPECT_TRUE( obj.set_value_via_path( "formats.add_object2.width", iwidth ) );
    EXPECT_TRUE( obj["formats"]["add_object2"].isObject() );
    EXPECT_FALSE( obj["formats"]["add_object2"].empty() );
    EXPECT_EQ( obj["formats"]["add_object2"]["width"].toInt(), iwidth );

    EXPECT_FALSE( obj.set_value_via_path( "data[4].value", 2.5 ) );
}

TEST( JsonioBase, SetOid )
{
    auto obj = json::loads("{ \"task\" : \"exampleCRUD\", "
                           "  \"properties\" : { \"level\": \"insert record\" } }");

    obj.set_oid("test/eCRUD");
    EXPECT_EQ( obj["_id"].toString(), "test/eCRUD" );
    EXPECT_EQ( obj["_key"].toString(), "eCRUD" );
    EXPECT_EQ( obj.dump(true), "{\"task\":\"exampleCRUD\",\"properties\":{\"level\":\"insert record\"},\"_key\":\"eCRUD\",\"_id\":\"test/eCRUD\"}\n" );

    obj.set_oid("");
    EXPECT_EQ( obj["_id"].toString(), "" );
    EXPECT_EQ( obj["_key"].toString(), "" );
    EXPECT_EQ( obj.dump(true), "{\"task\":\"exampleCRUD\",\"properties\":{\"level\":\"insert record\"}}\n" );
}
