#include <iostream>
#include "jsonparser.h"
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

         // Simple regular expression matching
            const std::string fnames[] = {"foo.txt", "-bar.txt", "baz.dat", "2zoidberg"};
            const std::regex txt_regex("^[a-z].*");

            for (const auto &fname : fnames) {
                std::cout << fname << ": " << std::regex_match(fname, txt_regex) << '\n';
            }

        return 0;
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
    }
    catch(...)
    {
        std::cout <<  "unknown exception" <<  std::endl;
    }

    return 0;
}


/**
 *
 *
 *
 *
        // test split
        auto all_slop16 = read_all_file( "slop16.dat" );
        //std::cout << json::dump(all_slop16) << std::endl;

        auto headers = regexp_extract(all_slop16, "\\*+[^\n]*\n\\s*([\\w ]+)\\s*\n\\*+[^\n]*\n");
        for( auto block: headers )
        {
            trim(block, "*\n\t ");
            std::cout << block << std::endl;
        }
        auto datas = regexp_split(all_slop16, "\\*+[^\n]*\n\\s*([\\w ]+)\\s*\n\\*+[^\n]*\n");

        // fix last
        auto top_data = datas.back();
        auto pos = top_data.rfind("-----------");
        if( pos != std::string:: npos )
        {
            top_data.resize(pos);
            pos = top_data.rfind("\n");

            while( pos!= std::string:: npos )
            {
                top_data.resize(pos);
                pos = top_data.rfind("\n");
                auto line = top_data.substr(pos);
                trim(line);
                if( line .empty())
                    break;
            }
            datas.back() = top_data;
        }

        /*for( auto block: datas )
        {
            std::cout << block  << "\n******************" << std::endl;
        }*
 *
 *
 *
 *
 *
 * */
