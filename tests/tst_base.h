#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsonbase.h"

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
    EXPECT_EQ( JsonBase::Int, JsonBase::typeTraits( vchar ) );
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
