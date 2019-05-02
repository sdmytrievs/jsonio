#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "jsonbuilder.h"
#include "jsonfree.h"

using namespace testing;
using namespace jsonio14;


TEST( JsonioParser, TestNullBool )
{
    auto jsFree = json::loads("[ null ]");
    EXPECT_EQ( JsonBase::Null, jsFree[0].getType() );

    jsFree = json::loads("[true]");
    EXPECT_EQ( JsonBase::Bool, jsFree[0].getType() );
    EXPECT_TRUE(jsFree[0].toBool());

    jsFree = json::loads("[false]");
    EXPECT_EQ( JsonBase::Bool, jsFree[0].getType() );
    EXPECT_FALSE(jsFree[0].toBool());
}

TEST( JsonioParser, TestDouble )
{
    auto TEST_DOUBLE = [](const std::string & json_string, const double expected)
    {
        auto jsFree = json::loads(json_string);
        EXPECT_EQ( JsonBase::Double, jsFree[0].getType() );
        EXPECT_NEAR( expected, jsFree[0].toDouble(), 1e-20 );
    };

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
    TEST_DOUBLE("[2.22507e-308]", 2.22507e-308);
    TEST_DOUBLE("[-1.79769e+308]", -1.79769e+308);
    TEST_DOUBLE("[-2.22507e-308]", -2.22507e-308);

    TEST_DOUBLE("[4.9406564584124654e-324]", 4.9406564584124654e-324); // minimum denormal
    TEST_DOUBLE("[2.2250738585072009e-308]", 2.2250738585072009e-308); // Max subnormal double
    TEST_DOUBLE("[2.2250738585072014e-308]", 2.2250738585072014e-308); // Min normal positive double
    TEST_DOUBLE("[1.7976931348623157e+308]", 1.7976931348623157e+308); // Max double
    TEST_DOUBLE("[1e-10000]", 0.0);                                   // must underflow

    TEST_DOUBLE("[1e-214748363]", 0.0);
    TEST_DOUBLE("[1e-214748364]", 0.0);

    TEST_DOUBLE("[0.017976931348623157e+310]", 1.7976931348623157e+308); // Max double in another form

}

TEST( JsonioParser, TestInt )
{
    auto TEST_INT = [](const std::string & json_string, const long expected)
    {
        auto jsFree = json::loads(json_string);
        EXPECT_EQ( JsonBase::Int, jsFree[0].getType() );
        EXPECT_EQ( expected, jsFree[0].toInt() );
    };

    TEST_INT("[0]", 0);
    TEST_INT("[-0]", -0);
    TEST_INT("[1]", 1);
    TEST_INT("[-100]", -100);
    TEST_INT("[+10]", 10);
    TEST_INT("[1000000]", 1000000);
}
