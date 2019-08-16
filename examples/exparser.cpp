#include <iostream>
#include "jsondump.h"
#include "jsonfree.h"
#include "service.h"
using namespace jsonio14;
using namespace std;

#include <regex>

int main(int argc, char* argv[])
{
    string test_dir = ".";
    if( argc > 1)
        test_dir = argv[1];

    try{

        // test all json files into directory
        auto fail_json_files =  files_into_directory( test_dir, ".json" );
        for( auto file: fail_json_files)
        {
            try{
                auto json_data = read_all_file( file );
                //std::cout <<  "\nStart: " << file << "'" << json_data<< "'\n";

                auto jsFree =  json::loads( json_data );
                //std::cout <<  "Pass: " <<  " Result: "  <<  json::dump( jsFree, true) <<  std::endl;
                std::cout <<  "Pass: " <<  file <<  std::endl;
            }
            catch( jarango_exception& e )
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

