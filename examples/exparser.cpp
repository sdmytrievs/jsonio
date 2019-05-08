#include <iostream>
#include "jsonparser.h"
#include "jsonfree.h"
#include "service.h"
using namespace jsonio14;
using namespace std;

int main(int argc, char* argv[])
{
    string test_dir = ".";
    if( argc > 1)
        test_dir = argv[1];

    try{

        auto fail_json_files =  files_into_directory( test_dir, ".json" );
        for( auto file: fail_json_files)
        {
            try{
                auto json_data = read_all_file( file );
                std::cout <<  "\nStart: " << file << "'" << json_data<< "'\n";

                auto jsFree =  json::loads( json_data );
                std::cout <<  "Pass: " <<  " Result: "  <<  json::dump( jsFree, true) <<  std::endl;
            }
            catch(jarango_exception& e)
            {
                std::cout << "Fail: " <<  " Exeption: "  <<  e.what() <<  std::endl;
            }
            catch(std::exception& e)
            {
                std::cout << "Fail: " << " std::exception: " << e.what() <<  std::endl;
            }
        }

        // test split
        auto all_slop16 = read_all_file( "slop16.dat" );
        std::cout << all_slop16 << std::endl;

    }
    catch(...)
    {
        std::cout <<  "unknown exception" <<  std::endl;
    }

    return 0;
}
