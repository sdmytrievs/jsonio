#pragma once

#include <gtest/gtest.h>

#include "jsonio17/jsonfree.h"
#include "jsonio17/service.h"
#include "jsonio17/txt2file.h"
#include "jsonio17/jsondump.h"

using namespace testing;
using namespace jsonio17;

const char* const test_free_str = R"([
                                  "JSON Test Pattern pass1",
                                  {"object_with_1_member":["array with 1 element"]},
                                  {},
                                  [],
                                  -42,
                                  true,
                                  false,
                                  {
                                      "integer": 1234567890,
                                      "real": -9876.543210,
                                      "e": 0.123456789e-12,
                                      "E": 1.234567890E+34,
                                      "":  23456789012E66,
                                      "zero": 0,
                                      "one": 1,
                                      "space": " ",
                                      "quote": "\"",
                                      "backslash": "\\",
                                      "controls": "\b\f\n\r\t",
                                      "slash": "/ & \/",
                                      "alpha": "abcdefghijklmnopqrstuvwyz",
                                      "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
                                      "digit": "a0123456789",
                                      "a123456789": "digit",
                                      "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
                                      "hex": "ģ䕧覫췯ꯍ",
                                      "true": true,
                                      "false": false,
                                      "null": null,
                                      "array":[  ],
                                      "object":{  },
                                      "address": "50 St. James Street",
                                      "url": "http://www.JSON.org/",
                                      "comment": "// /* <!-- --",
                                      "spaced" :[1,2 , 3

                              ,

                              4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
                                      "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
                                      "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
                                      "uuu": "A key can be any string"
                                  },
                                  0.5 ,98.6
                              ,
                              99.44
                              ,

                              1066,
                              1e1,
                              0.1e1,
                              1e-1,
                              1e00,2e+00,2e-00
                              ,"rosebud"]
)";

TEST( JsonioParser, TestNullBool )
{
    auto jsFree = json::loads("[ null ]");
    EXPECT_EQ( JsonBase::Null, jsFree[0].type() );

    jsFree = json::loads("[true]");
    EXPECT_EQ( JsonBase::Bool, jsFree[0].type() );
    EXPECT_TRUE(jsFree[0].toBool());

    jsFree = json::loads("[false]");
    EXPECT_EQ( JsonBase::Bool, jsFree[0].type() );
    EXPECT_FALSE(jsFree[0].toBool());
}

TEST( JsonioParser, TestDouble )
{
    auto TEST_DOUBLE = [](const std::string & json_string, const double expected)
    {
        auto jsFree = json::loads(json_string);
        EXPECT_EQ( JsonBase::Double, jsFree[0].type() );
        EXPECT_NEAR( expected, jsFree[0].toDouble(), 1e-20 );
    };

    // limits long int +9223372036854775807
    DetailSettings::doublePrecision = 15;
    TEST_DOUBLE("[+92233720368547758070]", 9.22337203685478e+19);

    TEST_DOUBLE("[.1]", 0.1);
    TEST_DOUBLE("[-.1]", -0.1);

    TEST_DOUBLE("[0.0]", 0.0);
    TEST_DOUBLE("[-0.0]", -0.0);
    TEST_DOUBLE("[1.0]", 1.0);
    TEST_DOUBLE("[-1.0]", -1.0);
    TEST_DOUBLE("[1.5]", 1.5);
    TEST_DOUBLE("[-1.5]", -1.5);
    TEST_DOUBLE("[3.1416]", 3.1416);
    TEST_DOUBLE("[1E10]", 1E10);
    TEST_DOUBLE("[1e10]", 1e10);
    TEST_DOUBLE("[1E+10]", 1E+10);
    TEST_DOUBLE("[1E-10]", 1E-10);
    TEST_DOUBLE("[-1E10]", -1E10);
    TEST_DOUBLE("[-1e10]", -1e10);
    TEST_DOUBLE("[-1E+10]", -1E+10);
    TEST_DOUBLE("[-1E-10]", -1E-10);
    TEST_DOUBLE("[1.234E+10]", 1.234E+10);
    TEST_DOUBLE("[1.234E-10]", 1.234E-10);
    TEST_DOUBLE("[1.79769e+308]", 1.79769e+308);
#ifndef __APPLE__
    TEST_DOUBLE("[2.22507e-308]", 2.22507e-308);
    TEST_DOUBLE("[-1.79769e+308]", -1.79769e+308);
    TEST_DOUBLE("[-2.22507e-308]", -2.22507e-308);

    TEST_DOUBLE("[4.9406564584124654e-324]", 4.9406564584124654e-324); // minimum denormal
    TEST_DOUBLE("[2.2250738585072009e-308]", 2.2250738585072009e-308); // Max subnormal double
    TEST_DOUBLE("[2.2250738585072014e-308]", 2.2250738585072014e-308); // Min normal positive double
#ifndef _MSC_VER
    TEST_DOUBLE("[1.7976931348623157e+308]", 1.7976931348623157e+308); // Max double

    TEST_DOUBLE("[1e-10000]", 0.0);                                   // must underflow
    TEST_DOUBLE("[1e-214748363]", 0.0);
    TEST_DOUBLE("[1e-214748364]", 0.0);

    TEST_DOUBLE("[0.017976931348623157e+310]", 1.7976931348623157e+308); // Max double in another form
#endif
#endif

}

TEST( JsonioParser, TestInt )
{
    auto TEST_INT = [](const std::string & json_string, const long expected)
    {
        auto jsFree = json::loads(json_string);
        EXPECT_EQ( JsonBase::Int, jsFree[0].type() );
        EXPECT_EQ( expected, jsFree[0].toInt() );
    };

    TEST_INT("[0]", 0);
    TEST_INT("[-0]", -0);
    TEST_INT("[1]", 1);
    TEST_INT("[-100]", -100);
    TEST_INT("[+10]", 10);
    TEST_INT("[1000000]", 1000000);

    auto TEST_LONG = [](const std::string & json_string, const long long expected)
    {
        auto jsFree = json::loads(json_string);
        if( expected > std::numeric_limits<long>::max() ||
            expected < std::numeric_limits<long>::min() )
        {
            EXPECT_EQ( JsonBase::Double, jsFree[0].type() );
            EXPECT_DOUBLE_EQ( static_cast<double>(expected), jsFree[0].toDouble() );
        }
        else
        {
            EXPECT_EQ( JsonBase::Int, jsFree[0].type() );
            EXPECT_EQ( expected, jsFree[0].toInt() );
        }
    };

    // limits
   TEST_LONG("[-9223372036854775807]", -9223372036854775807);
   TEST_LONG("[+9223372036854775807]", +9223372036854775807);
}


TEST( JsonioParser, TestString )
{
    auto TEST_STRING = [](const std::string & json_string, const std::string& expected)
    {
        auto jsFree = json::loads(json_string);
        EXPECT_EQ( JsonBase::String, jsFree[0].type() );
        EXPECT_EQ( expected, jsFree[0].toString() );
    };

    TEST_STRING("[ \"\" ]", "");
    TEST_STRING("[ \"abc\" ]", "abc" );

    TEST_STRING("[ \"abc dfg \" ]", "abc dfg ");
    TEST_STRING("[ \"\\\\ \\b\\f\\n\\r\\t \\\"\" ]", "\\ \b\f\n\r\t \"" );
}

TEST( JsonioParser, TestObject )
{
    auto jsFree = json::loads("{}");
    EXPECT_EQ( JsonBase::Object, jsFree.type() );

    jsFree = json::loads(" { \"abc\": 11 } ");
    EXPECT_EQ( JsonBase::Object, jsFree.type() );
    EXPECT_EQ( jsFree["abc"].toInt(), 11 );

    jsFree = json::loads(" { \"abc\": 11, \"dfg\": [ true ] } ");
    EXPECT_EQ( JsonBase::Object, jsFree.type() );
    EXPECT_TRUE( jsFree["dfg"][0].toBool() );
}

TEST( JsonioParser, TestArray )
{
    auto jsFree = json::loads("[]");
    EXPECT_EQ( JsonBase::Array, jsFree.type() );

    jsFree = json::loads(" [ \"abc\", 11, 12 ] ");
    EXPECT_EQ( JsonBase::Array, jsFree.type() );
    EXPECT_EQ( jsFree[1].toInt(), 11 );

    jsFree = json::loads(" [ [ true, false ] ] ");
    EXPECT_EQ( JsonBase::Array, jsFree.type() );
    EXPECT_TRUE( jsFree[0][0].toBool() );
}

TEST( JsonioParser, TestFail )
{
  EXPECT_THROW( json::loads( "[" ) , std::exception );

  auto fail_json_files =  files_into_directory( "fail" );
  for( auto file: fail_json_files)
  {
     auto json_data = read_ascii_file( file );
     EXPECT_THROW( json::loads( json_data ), jsonio_exception );
  }
}

TEST( JsonioParser, TestPass )
{
  EXPECT_NO_THROW( json::loads( "[ ] " ) );
  EXPECT_NO_THROW( json::loads( " {} " ) );

  auto fail_json_files =  files_into_directory( "pass" );
  for( auto const& file: fail_json_files)
  {
     auto json_data = read_ascii_file( file );
     EXPECT_NO_THROW( json::loads( json_data )  );
  }
}

TEST( JsonioParser, ObjectFree)
{
    auto json_obj = json::loads( test_free_str );
}
