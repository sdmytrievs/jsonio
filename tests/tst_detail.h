#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsondetail.h"
#include "exceptions.h"
#include "type_test.h"

using namespace testing;
using namespace jsonio14;


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

//------------------------------------------------------------------------------------

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
    EXPECT_EQ( "2.5" , v2string(2.5) );

    EXPECT_EQ( DetailSettings::infiniteValue , v2string(NAN) );
    EXPECT_EQ( DetailSettings::infiniteValue , v2string(INFINITY) );

    double f =3.14159;
    auto old = DetailSettings::doublePrecision;
    DetailSettings::doublePrecision = 3;
    EXPECT_EQ( "3.14" , v2string(f) );
    DetailSettings::doublePrecision = old;
}


TEST( JsonioDetail, string2v )
{
    long lval;
    double dval;
    EXPECT_TRUE( string2v( "1000", lval ) );
    EXPECT_EQ( lval, 1000L );
    EXPECT_TRUE( string2v( "1.5", dval ) );
    EXPECT_EQ( dval, 1.5 );
    EXPECT_TRUE( string2v( DetailSettings::infiniteValue, lval ) );
    EXPECT_EQ( lval, std::numeric_limits<long>::min() );
    EXPECT_TRUE( string2v( DetailSettings::infiniteValue, dval ) );
    EXPECT_EQ( dval, std::numeric_limits<double>::min() );

    bool bval;
    std::string sval;
    EXPECT_TRUE( string2v( "true", bval ) );
    EXPECT_EQ( bval, true );
    EXPECT_TRUE( string2v( "Test string", sval ) );
    EXPECT_EQ( sval, "Test string" );

    EXPECT_TRUE( string2v( DetailSettings::infiniteValue, bval ) );
    EXPECT_EQ( bval, false );
    EXPECT_TRUE( string2v( DetailSettings::infiniteValue, sval ) );
    EXPECT_EQ( sval, DetailSettings::infiniteValue );
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

TEST( JsonioDetail, Split )
{
 auto intquery =   split_int( "1---2---3", "---" );
 EXPECT_EQ( 3, intquery.size() );
 EXPECT_EQ( 1, intquery.front() );
 intquery.pop();
 EXPECT_EQ( 2, intquery.front() );
 intquery.pop();
 EXPECT_EQ( 3, intquery.front() );

 EXPECT_THROW( split_int( "1;;3", ";" ), jarango_exception );
 EXPECT_THROW( split_int( ";1e;3;", ";" ), jarango_exception );

 auto strquery = split("a:bb:ccc", ":" );
 EXPECT_EQ( 3, strquery.size() );
 EXPECT_EQ( "a", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "bb", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "ccc", strquery.front() );

 auto strquery2 = split("::bb::ccc::", ":" );
 EXPECT_EQ( 2, strquery2.size() );

 strquery = split("name1.name2.name3", "./[]" );
 EXPECT_EQ( 3, strquery.size() );
 EXPECT_EQ( "name1", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name2", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name3", strquery.front() );

 strquery = split("name1.name2[3].name3", "./[]" );
 EXPECT_EQ( 4, strquery.size() );
 EXPECT_EQ( "name1", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name2", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "3", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name3", strquery.front() );

 strquery = split("/name1/name2[3]/name3", "./[]" );
 EXPECT_EQ( 4, strquery.size() );
 EXPECT_EQ( "name1", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name2", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "3", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name3", strquery.front() );

 strquery = split("/name1/name2/3/name3", "./[]" );
 EXPECT_EQ( 4, strquery.size() );
 EXPECT_EQ( "name1", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name2", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "3", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name3", strquery.front() );

 strquery = split("[\"name1\"][\"name2\"][3][\"name3\"]", "./[]\"" );
 EXPECT_EQ( 4, strquery.size() );
 EXPECT_EQ( "name1", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name2", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "3", strquery.front() );
 strquery.pop();
 EXPECT_EQ( "name3", strquery.front() );

}
