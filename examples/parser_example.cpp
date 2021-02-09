#include <iostream>
#include "jsonio17/jsondump.h"
#include "jsonio17/txt2file.h"
#include "jsonio17/jsonfree.h"
using namespace jsonio17;
using namespace std;

#include <regex>

int main(int argc, char* argv[])
{
    string test_dir = ".";
    test_dir = "pass";
    //test_dir = "fail";
    //test_dir = "nst_json_testsuite/test_parsing";
    //test_dir = "nst_json_testsuite2/test_parsing";

    if( argc > 1)
        test_dir = argv[1];

    try{

        // test all json files into directory
        auto fail_json_files =  files_into_directory( test_dir, ".json" );
        for( auto file: fail_json_files)
        {
            try{
                auto json_data = read_ascii_file( file );
                //std::cout <<  "\nStart: " << file << "'" << json_data<< "'\n";

                auto jsFree =  json::loads( json_data );
                //std::cout <<  "Pass: " <<  " Result: "  <<  jsFree.dump(true) <<  std::endl;
                std::cout <<  "Pass: " <<  file <<  std::endl;
            }
            catch( jsonio_exception& e )
            {
                std::cout << "Fail: " << file <<  "  Exeption: "  <<  e.what() <<  std::endl;
            }
            catch( std::exception& e )
            {
                std::cout << "Fail: " << file << " std::exception: " << e.what() <<  std::endl;
            }
        }
    }
    catch(...)
    {
        std::cout <<  "  unknown exception" <<  std::endl;
    }

    return 0;
}

