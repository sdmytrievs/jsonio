#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>


#include "service.h"
#include "txt2file.h"
#include "jsondump.h"
#include "exceptions.h"

using namespace testing;
using namespace jsonio14;

//  Function that can be used to split text using regexp
TEST( JsonioService, regexpSplit )
{
    auto tokens = regexp_split( "1\r 22 \n333 ", "\\s+" );
    // std::cout << dump(tokens) << std::endl;
    EXPECT_EQ( "[ \"1\", \"22\", \"333\" ]", json::dump(tokens) );
    auto strquery = regexp_split("a:bb:ccc", ":" );
    EXPECT_EQ( "[ \"a\", \"bb\", \"ccc\" ]", json::dump(strquery) );
}

//  Function that can be used to extract tokens using regexp
TEST( JsonioService, regexpExtract )
{
    auto tokens = regexp_extract( " %h11 %h22 hhh %h33 ", "%h\\d+" );
    //std::cout << dump(tokens) << std::endl;
    EXPECT_EQ( "[ \"%h11\", \"%h22\", \"%h33\" ]", json::dump(tokens) );
}

//  Function that can be used to replase text using regexp
TEST( JsonioService, regexpReplace )
{
    auto resstr = regexp_replace("there is a subsequence in the string" ,"\\b(sub)([^ ]*)","sub-$2");
    EXPECT_EQ( "there is a sub-sequence in the string", resstr );
}


//  Function that can be used to replase text using regexp
TEST( JsonioService, regexprTest )
{
    EXPECT_TRUE( regexp_test("foo.txt", "^[a-z].*") );
    EXPECT_FALSE( regexp_test("-bar.txt", "^[a-z].*") );
    EXPECT_TRUE( regexp_test("baz.dat", "^[a-z].*") );
    EXPECT_FALSE( regexp_test("2zoidberg", "^[a-z].*") );
}

TEST( JsonioService, Trim )
{
    std::string str{" \t\rTest \n "};
    trim( str );
    EXPECT_EQ( "Test", str );
    str = ":: Test2 ;";
    trim( str, ": ;" );
    EXPECT_EQ( "Test2", str );
}

TEST( JsonioService, Replace )
{
    std::string str{"Test \t\n:"};
    replace_all( str, " \t\n", '_' );
    EXPECT_EQ( "Test___:", str );
}

// Test filesystem ----------------------------------------------------------------------

#include <fstream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

TEST( Jsoniofilesystem, HomeDir )
{
   auto current_path =  fs::current_path();
   std::cout << "Current path is " << fs::current_path() << '\n';

   std::string homedir, rel_path;
   EXPECT_NO_THROW( homedir = home_dir());
#ifdef _WIN32
   EXPECT_EQ( homedir, current_path.root_path());
#else
   EXPECT_TRUE( current_path.native().rfind(homedir, 0) == 0 );
#endif
  rel_path = current_path.native().substr(homedir.length());
  rel_path = "~"+rel_path;

  EXPECT_EQ( current_path.native(), expand_home_dir( rel_path, homedir ));
  EXPECT_EQ( current_path.native(), expand_home_dir( rel_path, "" ));
}

TEST( Jsoniofilesystem, TestDir )
{
    std::string test_dir = "test_dir";
    if(path_exist( test_dir ) )
    fs::remove_all(test_dir);
    fs::create_directory(test_dir);
    std::ofstream(test_dir+"/file1.txt").put('a');
    std::ofstream(test_dir+"/file2.txt").put('b');
    std::ofstream(test_dir+"/file3.json").put('1');

    auto files = files_into_directory( test_dir, "txt");
    EXPECT_EQ( files.size(), 2);
    EXPECT_EQ( files[0], test_dir+"/file1.txt");
    EXPECT_EQ( files[1], test_dir+"/file2.txt");

}
