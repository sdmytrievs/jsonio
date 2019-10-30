#pragma once

#include <gtest/gtest.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsonio14/jsondump.h"

using namespace testing;
using namespace jsonio14;

TEST( JsonioDump, dumpValue )
{
    // see  tst_detail  v2string
}

TEST( JsonioDump, dumpString )
{
    EXPECT_EQ( "\"Test char *\"", json::dump("Test char *") );
    std::string str{"Test string"};
    EXPECT_EQ( "\"Test string\"", json::dump(str) );
    str = "\\ \b\f\n\r\t ";
    EXPECT_EQ( "\"\\\\ \\b\\f\\n\\r\\t \"", json::dump(str) );
}

TEST( JsonioDump, dumpVector )
{
    std::vector<double> dvec = { 1.5, 2.5, 3.5, 4.5 };
    std::list<std::string> slist = { "v1", "v2", "v3", "v4" };
    EXPECT_EQ( "[ 1.5, 2.5, 3.5, 4.5 ]", json::dump(dvec));
    EXPECT_EQ( "[ \"v1\", \"v2\", \"v3\", \"v4\" ]", json::dump(slist));
}

TEST( JsonioDump, dumpMap )
{
    std::map<std::string, bool> vmapb = { {"key1", true }, {"key2", false } };
    std::map<std::string, std::string> vmaps = { {"key1", "val4" }, {"key2", "val5" } };
    EXPECT_EQ( "{ \"key1\":true, \"key2\":false }", json::dump(vmapb));
    EXPECT_EQ( "{ \"key1\":\"val4\", \"key2\":\"val5\" }",json::dump(vmaps));
}

// Must be more tests about dump/undump strings
TEST( JsonioDump, TestUndumpString )
{
    std::string sdata("");
    json::undumpString(sdata);
}
