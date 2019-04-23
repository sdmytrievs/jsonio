#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsondetail.h"

using namespace testing;
using namespace jsonio14;

TEST( JsonioDetail, dumpString )
{
    EXPECT_EQ( "\"Test char *\"", dump("Test char *") );
    std::string str{"Test string"};
    EXPECT_EQ( "\"Test string\"", dump(str) );
    str = "\\ \b\f\n\r\t ";
    EXPECT_EQ( "\"\\\\ \\b\\f\\n\\r\\t \"", dump(str) );
}

TEST( JsonioDetail, dumpVector )
{
    std::vector<double> dvec = { 1.5, 2.5, 3.5, 4.5 };
    std::list<std::string> slist = { "v1", "v2", "v3", "v4" };
    EXPECT_EQ( "[ 1.5, 2.5, 3.5, 4.5 ]", dump(dvec));
    EXPECT_EQ( "[ \"v1\", \"v2\", \"v3\", \"v4\" ]", dump(slist));
}

TEST( JsonioDetail, dumpMap )
{
    std::map<std::string, bool> vmapb = { {"key1", true }, {"key2", false } };
    std::map<std::string, std::string> vmaps = { {"key1", "val4" }, {"key2", "val5" } };
    EXPECT_EQ( "{ \"key1\":true, \"key2\":false }", dump(vmapb));
    EXPECT_EQ( "{ \"key1\":\"val4\", \"key2\":\"val5\" }",dump(vmaps));
}

TEST( JsonioDetail, v2string )
{
    EXPECT_EQ( "Test char *", v2string("Test char *") );
    std::string str{"Test string"};
    EXPECT_EQ( str, v2string(str) );
    EXPECT_EQ( "n", v2string('n') );
    unsigned char ch = 'Q';
    EXPECT_EQ( "Q", v2string(ch)  );

    EXPECT_EQ( "true" , v2string(true) );
    EXPECT_EQ( "false" , v2string(false) );
    EXPECT_EQ( std::to_string(10) , v2string(10) );
    EXPECT_EQ( std::to_string(-10L) , v2string(-10L) );
    EXPECT_EQ( "1.5" , v2string(1.5f) );
    EXPECT_EQ( "1e-05" , v2string(1e-5) );
}


TEST( JsonioDetail, string2v )
{
    long lval;
    double dval;
    EXPECT_TRUE( string2v( "1000", lval ) );
    EXPECT_EQ( lval, 1000L );
    EXPECT_TRUE( string2v( "1.5", dval ) );
    EXPECT_EQ( dval, 1.5 );
    EXPECT_TRUE( string2v( "null", lval ) );
    EXPECT_EQ( lval, std::numeric_limits<long>::min() );
    EXPECT_TRUE( string2v( "null", dval ) );
    EXPECT_EQ( dval, std::numeric_limits<double>::min() );

    bool bval;
    std::string sval;
    EXPECT_TRUE( string2v( "true", bval ) );
    EXPECT_EQ( bval, true );
    EXPECT_TRUE( string2v( "Test string", sval ) );
    EXPECT_EQ( sval, "Test string" );

    EXPECT_TRUE( string2v( "null", bval ) );
    EXPECT_EQ( bval, false );
    EXPECT_TRUE( string2v( "null", sval ) );
    EXPECT_EQ( sval, "null" );
}


TEST( JsonioDetail, isType )
{
    long lval;
    double dval;

    EXPECT_TRUE( is<long>( lval, "1000" ) );
    EXPECT_EQ( lval, 1000L );

    EXPECT_TRUE( is<double>( dval, "1.5" ) );
    EXPECT_EQ( dval, 1.5 );
    EXPECT_TRUE( is<double>( dval, "-1e-5" ) );
    EXPECT_EQ( dval, -1e-5 );

    EXPECT_FALSE( is<long>( lval, "1.5" ) );
    EXPECT_FALSE( is<long>( lval, "e1" ) );
    EXPECT_FALSE( is<long>( lval, "true" ) );
}

TEST( JsonioDetail, isContainer )
{
    // std::cout << "Test is_container" << std::endl;
    EXPECT_TRUE( is_container<std::forward_list<int>>::value );
    EXPECT_TRUE( is_container<std::vector<int>>::value );
    EXPECT_TRUE( is_container<std::list<int>>::value );
    EXPECT_TRUE( is_container<std::set<int>>::value );
    EXPECT_TRUE( is_container<std::unordered_set<int>>::value );
    EXPECT_TRUE( is_container<std::multiset<int>>::value );

    EXPECT_FALSE( is_container<std::queue<int>>::value );  // !!!!

    auto map = is_container<std::map<std::string, int>>::value;
    EXPECT_TRUE( map  );
    auto unordered = is_container<std::unordered_map<std::string, int>>::value;
    EXPECT_TRUE( unordered );
    auto multi = is_container<std::multimap<std::string, int>>::value;
    EXPECT_TRUE( multi );

    EXPECT_FALSE( is_container<int>::value );
    EXPECT_FALSE( is_container<double>::value );
    EXPECT_TRUE( is_container<std::string>::value );      //true
}

TEST( JsonioDetail, isMappish )
{
    // std::cout << "Test is_mappish" << std::endl;
    EXPECT_FALSE( is_mappish<std::forward_list<int>>::value );
    EXPECT_FALSE( is_mappish<std::vector<int>>::value );
    EXPECT_FALSE( is_mappish<std::list<int>>::value );
    EXPECT_FALSE( is_mappish<std::set<int>>::value );
    EXPECT_FALSE( is_mappish<std::unordered_set<int>>::value );
    EXPECT_FALSE( is_mappish<std::multiset<int>>::value );

    auto map = is_mappish<std::map<std::string, int>>::value;
    EXPECT_TRUE( map  );
    auto unordered = is_mappish<std::unordered_map<std::string, int>>::value;
    EXPECT_TRUE( unordered );

    auto multi = is_mappish<std::multimap<std::string, int>>::value;
    EXPECT_FALSE( multi );     // !!!!
    auto multiunordered = is_mappish<std::unordered_multimap<int, double>>::value;
    EXPECT_FALSE( multiunordered );    // !!!!

    EXPECT_FALSE( is_mappish<int>::value );
    EXPECT_FALSE( is_mappish<double>::value );
    EXPECT_FALSE( is_mappish<std::string>::value );
}

TEST( JsonioDetail, Trim )
{
    std::string str{" \t\rTest \n "};
    trim( str );
    EXPECT_EQ( "Test", str );
    str = ":: Test2 ;";
    trim( str, ": ;" );
    EXPECT_EQ( "Test2", str );
}

TEST( JsonioDetail, Replace )
{
    std::string str{"Test \t\n:"};
    replace_all( str, " \t\n", '_' );
    EXPECT_EQ( "Test___:", str );
}

TEST( JsonioDetail, Split )
{
 auto intquery =   split_int( "1;2;3", ";" );
 EXPECT_EQ( 3, intquery.size() );
 EXPECT_EQ( 1, intquery.front() );
 intquery.pop();
 EXPECT_EQ( 2, intquery.front() );
 intquery.pop();
 EXPECT_EQ( 3, intquery.front() );

 auto intquery2 =   split_int( "1;;3", ";" );
 EXPECT_EQ( 2, intquery2.size() );
 auto intquery3 =   split_int( ";1e;3;", ";" );
 EXPECT_EQ( 1, intquery3.size() );

 auto strquery = split("a:bb:ccc", ":" );
 EXPECT_EQ( 3, strquery.size() );
 EXPECT_EQ( "a", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "bb", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "ccc", strquery.front() );

 auto strquery2 = split("::bb::ccc::", "::" );
 EXPECT_EQ( 7, strquery2.size() );
}
