#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>


#include "service.h"

using namespace testing;
using namespace jsonio14;

TEST( JsonioService, regexpSplit )
{
    //  Function that can be used to split text using regexp
    // std::vector<std::string> regexp_split(const std::string& str, std::string rgx_str = "\\s+");

}

TEST( JsonioService, regexpExtract )
{
    //  Function that can be used to extract tokens using regexp
    //  std::vector<std::string> regexp_extract(const std::string& str, std::string rgx_str);

//vector<string> tokens = regexp_extract( format.head_regexp, "%head\\d+");
//tokens = regexp_extract( format.end_regexp, "%end\\d+");
}

TEST( JsonioService, regexpReplace )
{
    //  Function that can be used to replase text using regexp
    // std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement );
//regexp_replace( format.key_regexp, "%key", impexKey );
//regexp_replace( rgex[ii++], "%value", values[0] );
}
