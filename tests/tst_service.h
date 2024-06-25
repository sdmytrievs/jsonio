#pragma once

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "jsonio17/service.h"
#include "jsonio17/txt2file.h"
#include "jsonio17/jsondump.h"
#include "jsonio17/exceptions.h"
#include "jsonio17/io_settings.h"

using namespace testing;
using namespace jsonio17;

//  Function that can be used to split text using regexp
TEST( JsonioService, regexpSplit )
{
    auto tokens = regexp_split( "1\r 22 \n333 ", "\\s+" );
    EXPECT_EQ( "[ \"1\", \"22\", \"333\" ]", json::dump(tokens) );
    auto strquery = regexp_split("a:bb:ccc", ":" );
    EXPECT_EQ( "[ \"a\", \"bb\", \"ccc\" ]", json::dump(strquery) );

    tokens = regexp_split( " \"aaa\", \"bbb\", \"ccc\" ", "[\",\\s]+" );
    EXPECT_EQ( "[ \"\", \"aaa\", \"bbb\", \"ccc\" ]", json::dump(tokens) );
}

//  Function that can be used to extract tokens using regexp
TEST( JsonioService, regexpExtract )
{
    auto tokens = regexp_extract( " %h11 %h22 hhh %h33 ", "%h\\d+" );
    EXPECT_EQ( "[ \"%h11\", \"%h22\", \"%h33\" ]", json::dump(tokens) );

    tokens = regexp_extract(
                "\"limitsTP\":null,\"m_compressibility\":10,\"m_expansivity\":null,\"name\":\"Al(OH)4-\",\"reaction\":null,",
                "(?!,)[^,\n]+(?=null,)" );
    EXPECT_EQ( "[ \"\\\"limitsTP\\\":\", \"\\\"m_expansivity\\\":\", \"\\\"reaction\\\":\" ]", json::dump(tokens) );
    tokens = regexp_extract( " \"aaa\", \"bbb\", \"ccc\" ", "[^\",\\s]+" );
    EXPECT_EQ( "[ \"aaa\", \"bbb\", \"ccc\" ]", json::dump(tokens) );

}

//  Function that can be used to replase text using regexp
TEST( JsonioService, regexpReplace )
{
    auto fieldstr = regexp_replace("field1.values.0" ,"\\.([0-9])", "[$1]");
    EXPECT_EQ( "field1.values[0]", fieldstr );
    fieldstr = regexp_replace("field1.values.0.next" ,"\\.([0-9])", "[$1]");
    EXPECT_EQ( "field1.values[0].next", fieldstr );

    fieldstr = regexp_replace("field1.values[0]" ,"\\[([0-9])+\\]", ".$1");
    EXPECT_EQ( "field1.values.0", fieldstr );
    fieldstr = regexp_replace("field1.values[0].next" ,"\\[([0-9])+\\]", ".$1");
    EXPECT_EQ( "field1.values.0.next", fieldstr );

    auto resstr = regexp_replace("there is a subsequence in the string" ,"\\b(sub)([^ ]*)","sub-$2");
    EXPECT_EQ( "there is a sub-sequence in the string", resstr );

    std::string rev = "{\"_id\":\"test_vertex_API/eCreate\",\"_key\":\"eCreate\",\"_rev\":\"_aGZ9am----\",\"task\":\"exampleCRUD\"}";
    auto resrev = regexp_replace(rev ,"(\"_rev\":\"[^\"]*\",)","");
    EXPECT_EQ( "{\"_id\":\"test_vertex_API/eCreate\",\"_key\":\"eCreate\",\"task\":\"exampleCRUD\"}", resrev );

    rev = "Al1;{\"0\":\"ELEMENT\"};{\"15\":\"PSI_NAGRA\"}";
    resrev = regexp_replace(rev ,"\"*:\"*",":");
    resrev = regexp_replace(resrev ,"\\{\"*","");
    resrev = regexp_replace(resrev ,"\"*\\}","");
    EXPECT_EQ( "Al1;0:ELEMENT;15:PSI_NAGRA", resrev );

    rev = "PSI_SeO4<2->_SO4<2->_Na<+>";
    resrev = regexp_replace(rev ,"<[^>]*>","");
    EXPECT_EQ( "PSI_SeO4_SO4_Na", resrev );

}


//  Function that can be used to replase text using regexp
TEST( JsonioService, regexprTest )
{
    EXPECT_TRUE( regexp_test("foo.txt", "^[a-z].*") );
    EXPECT_FALSE( regexp_test("-bar.txt", "^[a-z].*") );
    EXPECT_TRUE( regexp_test("baz.dat", "^[a-z].*") );
    EXPECT_FALSE( regexp_test("2zoidberg", "^[a-z].*") );
}


TEST( JsonioService, regexprExtractString )
{
    std::string test_data = R"({\n "_key"  : "eCRUD"    ,"properties":{"level": "insert record"},"task":
                            "exampleCRUD"})";

    EXPECT_EQ( regexp_extract_string(".*\"_key\"\\s*:\\s*\"([^\"]*)\".*", test_data), "eCRUD" );
    EXPECT_EQ( regexp_extract_string(".*\"level\"\\s*:\\s*\"([^\"]*)\".*", test_data), "insert record" );
    EXPECT_EQ( regexp_extract_string(".*\"task\"\\s*:\\s*\"([^\"]*)\".*", test_data), "exampleCRUD" );
}


TEST( JsonioService, extractStringJson )
{
    std::string test_data = R"({"_id":"test/eCRUD",  "_key":
                                "eCRUD" ,"properties":{"level":"insert record"},"task":"exampleCRUD"})";
    EXPECT_EQ( extract_string_json("_id", test_data), "test/eCRUD" );
    EXPECT_EQ( extract_string_json("_key", test_data), "eCRUD" );
    EXPECT_EQ( extract_string_json("level", test_data), "insert record" );
    EXPECT_EQ( extract_string_json("task", test_data), "exampleCRUD" );

    EXPECT_EQ( extract_string_json("notexist", test_data), "" );
    EXPECT_EQ( extract_int_json("task", test_data), 0 );

    test_data = R"({
                "_rev" :   "",
                "_type" :   "vertex",
                "_label" :   "element"
           })";
    EXPECT_EQ( extract_string_json("_type", test_data), "vertex" );
    EXPECT_EQ( extract_string_json("_label", test_data), "element" );

    std::string test_data2 = R"({"_key":"Kaolinite-dbr-0-0000","set":"Kaolinite","r":1})";
    EXPECT_EQ( extract_string_json("_key", test_data2), "Kaolinite-dbr-0-0000" );
    EXPECT_EQ( extract_string_json("set", test_data2), "Kaolinite" );

}

TEST( JsonioService, extractIntJson )
{
    std::string test_data = "{ \"i1\": +5,  \"i2\":-8\t,\"properties\":{\t\"level\"\n: 15},\"task\":\n\r\f\v 10 \t}";
    EXPECT_EQ( extract_int_json("i1", test_data), 5 );
    EXPECT_EQ( extract_int_json("i2", test_data), -8 );
    EXPECT_EQ( extract_int_json("level", test_data), 15 );
    EXPECT_EQ( extract_int_json("task", test_data), 10 );

    EXPECT_EQ( extract_string_json("task", test_data), "" );
    EXPECT_EQ( extract_int_json("notexist", test_data), 0 );
}


TEST( JsonioService, Trim )
{
    std::string str{" \t\rTest \n "};
    trim( str );
    EXPECT_EQ( "Test", str );
    str = ":: Test2 ;";
    trim( str, ": ;" );
    EXPECT_EQ( "Test2", str );
    str = "[{}]\n";
    trim( str );
    trim( str, "[]" );
    EXPECT_EQ( "{}", str );

}

TEST( JsonioService, Replace )
{
    std::string str{"Test \t\n:"};
    replace_all( str, " \t\n", '_' );
    EXPECT_EQ( "Test___:", str );
}


TEST( Jsoniofilesystem, HomeDir )
{
    auto current_path =  fs::current_path();
    io_logger->info("Current path is  {}", current_path.c_str());

    std::string homedir, rel_path;
    EXPECT_NO_THROW( homedir = home_dir());
    if( homedir.find("root") == std::string::npos) {
#ifdef _WIN32
        EXPECT_EQ( homedir, current_path.root_path().string());
#else
        EXPECT_TRUE( current_path.native().rfind(homedir, 0) == 0 );
#endif
        std::string newpath =current_path.string();
        rel_path = newpath.substr(homedir.length());
        rel_path = "~"+rel_path;

        EXPECT_EQ( current_path.string(), expand_home_dir( rel_path, homedir ));
        EXPECT_EQ( current_path.string(), expand_home_dir( rel_path, "" ));
    }
}

TEST( Jsoniofilesystem, TestDir )
{
    std::string test_dir = "test_dir";
#ifndef _WIN32
    if( path_exist( test_dir ) )
        fs::remove_all(test_dir);
#endif
    fs::create_directory(test_dir);
    std::ofstream(test_dir+"/file1.txt").put('a');
    std::ofstream(test_dir+"/file2.txt").put('b');
    std::ofstream(test_dir+"/file3.json").put('1');

    auto files = files_into_directory( test_dir, "txt");
    EXPECT_EQ( files.size(), static_cast<size_t>(2));
#ifdef _WIN32
    EXPECT_TRUE(std::find(files.begin(), files.end(),test_dir+"\\file1.txt")!=files.end() );
    EXPECT_TRUE(std::find(files.begin(), files.end(),test_dir+"\\file2.txt")!=files.end() );
#else
    EXPECT_TRUE(std::find(files.begin(), files.end(),test_dir+"/file1.txt")!=files.end() );
    EXPECT_TRUE(std::find(files.begin(), files.end(),test_dir+"/file2.txt")!=files.end() );
#endif
}

TEST( Jsoniofilesystem, TestTextFile )
{
    std::string fpath = "test.txt";
    std::string fdata = "TxtFile test data";

    std::ofstream stream;
    stream.open(fpath);
    stream << fdata;
    stream.close();

    TxtFile ftxt(fpath);
    EXPECT_EQ( ftxt.path(), fpath );
    EXPECT_EQ( ftxt.name(), "test" );
    EXPECT_EQ( ftxt.ext(), "txt" );
    EXPECT_EQ( ftxt.dir(), "");
    EXPECT_EQ( ftxt.type(), TxtFile::Txt );

    EXPECT_TRUE( ftxt.exist() );
    EXPECT_FALSE( ftxt.isOpened() );
    EXPECT_TRUE( ftxt.check_permission( TxtFile::ReadOnly ) );
    EXPECT_TRUE( ftxt.check_permission( TxtFile::WriteOnly ) );
    EXPECT_TRUE( ftxt.check_permission( TxtFile::ReadWrite ) );

    EXPECT_EQ( ftxt.load_all(), fdata );
}

TEST( Jsoniofilesystem, TestJsonFile )
{
    std::string fpath = "test.json";
    std::string fdata = "{\"width\":20,\"precision\":10}";

    std::ofstream stream;
    stream.open(fpath);
    stream << fdata;
    stream.close();

    JsonFile ftxt(fpath);
    EXPECT_EQ( ftxt.path(), fpath );
    EXPECT_EQ( ftxt.name(), "test" );
    EXPECT_EQ( ftxt.ext(), "json" );
    EXPECT_EQ( ftxt.dir(), "");
    EXPECT_EQ( ftxt.type(), TxtFile::Json );

    EXPECT_TRUE( ftxt.exist() );
    EXPECT_EQ( ftxt.load_all(), fdata );

    auto obj = JsonFree::object();
    EXPECT_NO_THROW( ftxt.load(obj) );
    EXPECT_EQ( ftxt.load_json(), obj.toString(true) );

    EXPECT_FALSE( ftxt.isOpened() );
}

TEST( Jsoniofilesystem, TestJsonFileWrite )
{
    std::string fpath = "testwrite.json";
    std::string fdata = "{\"width\":20,\"precision\":10}";
    auto obj = json::loads(fdata);

    JsonFile ftxt(fpath);
    EXPECT_EQ( ftxt.path(), fpath );
    EXPECT_EQ( ftxt.type(), TxtFile::Json );

    EXPECT_NO_THROW( ftxt.save(obj) );
    EXPECT_EQ( ftxt.load_json(), obj.toString(true) );

    EXPECT_FALSE( ftxt.isOpened() );
}


TEST( Jsoniofilesystem, TestJsonArrayFileSave )
{
    std::string fpath = "test_array_write.json";

    JsonArrayFile fjson(fpath);
    EXPECT_EQ( fjson.path(), fpath );
    EXPECT_EQ( fjson.type(), TxtFile::Json );
    EXPECT_NO_THROW( fjson.Open(TxtFile::WriteOnly) );

    for( size_t ii=0; ii<5; ii++)
        EXPECT_TRUE( fjson.saveNext( std::string("{ \"key\": ")+std::to_string(ii)+"}") );

    EXPECT_TRUE( fjson.isOpened() );
    EXPECT_NO_THROW( fjson.Close() );
    EXPECT_EQ( fjson.load_json(), "[{\"key\":0},{\"key\":1},{\"key\":2},{\"key\":3},{\"key\":4}]" );
}

TEST( Jsoniofilesystem, TestJsonArrayFileSaveSimple )
{
    std::string fpath = "test_array_write2.json";

    JsonArrayFile fjson(fpath);
    EXPECT_EQ( fjson.path(), fpath );
    EXPECT_EQ( fjson.type(), TxtFile::Json );
    EXPECT_NO_THROW( fjson.Open(TxtFile::WriteOnly) );

    for( size_t ii=0; ii<5; ii++)
        EXPECT_TRUE( fjson.saveNext( std::to_string(ii) ) );

    EXPECT_TRUE( fjson.isOpened() );
    EXPECT_NO_THROW( fjson.Close() );
    EXPECT_EQ( fjson.load_json(), "[0,1,2,3,4]" );
}


TEST( Jsoniofilesystem, TestJsonArrayFileLoad )
{
    std::string fpath = "test_array_load.json";
    std::string fdata = "[{\"key\":0},{\"key\":1},{\"key\":2},{\"key\":3},{\"key\":4}]";
    auto obj = JsonFree::object();

    std::ofstream stream;
    stream.open(fpath);
    stream << fdata;
    stream.close();

    JsonArrayFile fjson(fpath);
    EXPECT_EQ( fjson.path(), fpath );
    EXPECT_EQ( fjson.type(), TxtFile::Json );
    EXPECT_NO_THROW( fjson.Open(TxtFile::ReadOnly) );
    EXPECT_TRUE( fjson.isOpened() );

    for( size_t ii=0; ii<5; ii++)
    {
        EXPECT_TRUE( fjson.loadNext( obj ));
        EXPECT_EQ( obj.toString(true), std::string("{\"key\":")+std::to_string(ii)+"}" );
    }
    EXPECT_NO_THROW( fjson.Close() );
    EXPECT_EQ( fjson.load_json(), "[{\"key\":0},{\"key\":1},{\"key\":2},{\"key\":3},{\"key\":4}]" );
}

TEST( Jsoniofilesystem, TestJsonArrayFileLoadSimple )
{
    std::string fpath = "test_array_load2.json";
    std::string fdata = "[0,1,2,3,4]";
    auto obj = JsonFree::object();

    std::ofstream stream;
    stream.open(fpath);
    stream << fdata;
    stream.close();

    JsonArrayFile fjson(fpath);
    EXPECT_EQ( fjson.path(), fpath );
    EXPECT_EQ( fjson.type(), TxtFile::Json );
    EXPECT_NO_THROW( fjson.Open(TxtFile::ReadOnly) );
    EXPECT_TRUE( fjson.isOpened() );

    for( size_t ii=0; ii<5; ii++)
    {
        EXPECT_TRUE( fjson.loadNext( obj ));
        EXPECT_EQ( obj.toString(true), std::to_string(ii) );
    }
    EXPECT_NO_THROW( fjson.Close() );
    EXPECT_EQ( fjson.load_json(), fdata );
}


//---------------------------------------------------------------------

TEST( JsonioSettings, TestSettingsCreate )
{
    std::string fpath = "test1.cfg.json";
    if(path_exist( fpath ) )
        fs::remove_all(fpath);

    JsonioSettings  tst_settings( fpath );
    auto sec_test = tst_settings.section("common.test");
    EXPECT_FALSE( sec_test.contains("UseString") );
    EXPECT_FALSE( tst_settings.contains("common.test.UseBool") );

    sec_test.setValue("UseString","Test string" );
    sec_test.setValue("UseBool", true);
    sec_test.setValue("UseInt",1 );
    sec_test.setValue("UseDouble", 2.5 );

    EXPECT_TRUE( sec_test.contains("UseString") );
    EXPECT_TRUE( tst_settings.contains("common.test.UseBool") );

    JsonFile ftxt(fpath);
    EXPECT_EQ( ftxt.load_json(),
         "{\"jsonio\":{},\"log\":{\"modules\":[\"jsonarango\",\"jsonio17\"],\"level\":\"info\"},\"common\":{\"test\":{\"UseString\":\"Test string\",\"UseBool\":true,\"UseInt\":1,\"UseDouble\":2.5}}}" );

    if(path_exist( fpath ) )
        fs::remove_all(fpath);
}

TEST( JsonioSettings, TestSettingsRead )
{
    std::string fpath = "test2.cfg.json";
    std::string fdata = "{\"common\":{\"test\":{\"UseString\":\"Test string\",\"UseBool\":true,\"UseInt\":1,\"UseDouble\":2.5}}}";

    std::ofstream stream;
    stream.open(fpath);
    stream << fdata << std::endl;
    stream.close();

    JsonioSettings  tst_settings( fpath );
    auto sec_test = tst_settings.section("common.test");
    EXPECT_TRUE( sec_test.contains("UseString") );
    EXPECT_TRUE( tst_settings.contains("common.test.UseBool") );

    EXPECT_EQ( sec_test.value("UseString", std::string("default")), "Test string" );
    EXPECT_EQ( sec_test.value("UseBool", false), true );
    EXPECT_EQ( sec_test.value("UseInt", 5), 1 );
    EXPECT_EQ( sec_test.value("UseDouble", 3.8), 2.5 );

    if(path_exist( fpath ) )
        fs::remove_all(fpath);
}

TEST( JsonioSettings, TestSettingsPath )
{
    std::string fpath = "test3.cfg.json";

    JsonioSettings  tst_settings( fpath );
    std::string ahome_dir = home_dir();
    tst_settings.setHomeDir("~/newJSONIO/jsonio17" );
    tst_settings.setValue("common.ResourcesDirectory","~/Resources" );
    tst_settings.setValue("common.SchemasDirectory","~/Resources/data/schemas" );
    tst_settings.setUserDir("." );

    EXPECT_EQ( tst_settings.userDir(), "." );
#ifdef _WIN32
    EXPECT_EQ( tst_settings.homeDir(), "D:\\/newJSONIO/jsonio17" );
    EXPECT_EQ( tst_settings.resourcesDir(), "D:\\/newJSONIO/jsonio17/Resources" );
    EXPECT_EQ( tst_settings.directoryPath( "common.SchemasDirectory",  std::string("") ),
               "D:\\/newJSONIO/jsonio17/Resources/data/schemas" );
#else
    EXPECT_EQ( tst_settings.homeDir(), ahome_dir+"/newJSONIO/jsonio17" );
    EXPECT_EQ( tst_settings.resourcesDir(), ahome_dir+"/newJSONIO/jsonio17/Resources" );
    EXPECT_EQ( tst_settings.directoryPath( "common.SchemasDirectory",  std::string("") ),
               ahome_dir+"/newJSONIO/jsonio17/Resources/data/schemas" );
#endif

    JsonFile fjson(fpath);
    EXPECT_EQ( fjson.load_json(), "{\"jsonio\":{},\"log\":{\"modules\":[\"jsonarango\",\"jsonio17\"],\"level\":\"info\"},\"common\":{\"UserHomeDirectoryPath\":\"~/newJSONIO/jsonio17\","
                                  "\"ResourcesDirectory\":\"~/Resources\",\"SchemasDirectory\":\"~/Resources/data/schemas\","
                                  "\"WorkDirectoryPath\":\".\"}}" );
    if(path_exist( fpath ) )
        fs::remove_all(fpath);
}
