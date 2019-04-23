#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>


#include "service.h"
#include "jsondetail.h"

using namespace testing;
using namespace jsonio14;

//  Function that can be used to split text using regexp
TEST( JsonioService, regexpSplit )
{
    auto tokens = regexp_split( "1\r 22 \n333 ", "\\s+" );
    // std::cout << dump(tokens) << std::endl;
    EXPECT_EQ( "[ \"1\", \"22\", \"333\" ]", dump(tokens) );
    auto strquery = regexp_split("a:bb:ccc", ":" );
    EXPECT_EQ( "[ \"a\", \"bb\", \"ccc\" ]", dump(strquery) );
}

//  Function that can be used to extract tokens using regexp
TEST( JsonioService, regexpExtract )
{
    auto tokens = regexp_extract( " %h11 %h22 %h33 ", "%h\\d+" );
    //std::cout << dump(tokens) << std::endl;
    EXPECT_EQ( "[ \"%h11\", \"%h22\", \"%h33\" ]", dump(tokens) );
}

//  Function that can be used to replase text using regexp
TEST( JsonioService, regexpReplace )
{
    auto resstr = regexp_replace("there is a subsequence in the string" ,"\\b(sub)([^ ]*)","sub-$2");
    EXPECT_EQ( "there is a sub-sequence in the string", resstr );
}
