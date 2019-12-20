#include <iostream>
#include <chrono>

#include "jsonio14/jsondump.h"
#include "jsonio14/txt2file.h"
#include "jsonio14/jsonfree.h"

#include <velocypack/Parser.h>

#include <nlohmann/json.hpp>


void usage( const std::string& progname )
{
    std::cout <<  "Usage: " << progname << " [j|v|n] <directory> " << std::endl;
}

int jsonio14_parse_test( const std::vector<std::string>& json_files )
{
    for( auto file: json_files)
    {
        try{
            auto json_data = jsonio14::read_ascii_file( file );
            //std::cout <<  "\nStart: " << file << "'" << json_data<< "'\n";

            auto jsFree =  jsonio14::json::loads( json_data );
            //std::cout <<  "Pass: " <<  " Result: "  <<  jsFree.dump(true) <<  std::endl;
            std::cout <<  "Pass: " <<  file <<  std::endl;
        }
        catch( jsonio14::jarango_exception& e )
        {
            std::cout << "Fail: " << file <<  "  Exeption: "  <<  e.what() <<  std::endl;
        }
        catch( std::exception& e )
        {
            std::cout << "Fail: " << file << " std::exception: " << e.what() <<  std::endl;
        }
    }
    return 0;
}

int velocypack_parse_test( const std::vector<std::string>& json_files )
{
    for( auto file: json_files)
    {
        try{
            auto json_data = jsonio14::read_ascii_file( file );
            //std::cout <<  "\nStart: " << file << "'" << json_data<< "'\n";

            auto data = ::arangodb::velocypack::Parser::fromJson( json_data );
            //std::cout <<  "Pass: " <<  " Result: "  <<  data->slice().toJson() <<  std::endl;
            std::cout <<  "Pass: " <<  file <<  std::endl;
        }
        catch (::arangodb::velocypack::Exception& error )
        {
            std::cout << "Fail: " << file <<  "  Exeption: "  << error.what() << std::endl;
        }
        catch( std::exception& e )
        {
            std::cout << "Fail: " << file << " std::exception: " << e.what() <<  std::endl;
        }
    }
    return 0;
}

int nlohmann_parse_test( const std::vector<std::string>& json_files )
{
    for( auto file: json_files)
    {
        try{
            auto json_data = jsonio14::read_ascii_file( file );
            //std::cout <<  "\nStart: " << file << "'" << json_data<< "'\n";

            auto data = nlohmann::json::parse( json_data );
            //std::cout <<  "Pass: " <<  " Result: "  <<  data.dump() <<  std::endl;
            std::cout <<  "Pass: " <<  file <<  std::endl;
        }
        catch (nlohmann::detail::exception& error )
        {
            std::cout << "Fail: " << file <<  "  Exeption: "  << error.what() << std::endl;
        }
        catch( std::exception& e )
        {
            std::cout << "Fail: " << file << " std::exception: " << e.what() <<  std::endl;
        }
    }
    return 0;
}


int main(int argc, char* argv[])
{
    char parser = 'v';
    std::string test_dir = ".";
    //test_dir = "pass";
    test_dir = "fail";
    //test_dir = "nst_json_testsuite/test_parsing";
    //test_dir = "nst_json_testsuite2/test_parsing";

    if( argc < 2)
        usage( argv[0]);

    if( argc > 1)
        parser = argv[1][0];

    if( argc > 2)
        test_dir = argv[2];

    try{

        switch( parser )
        {
        case 'j':  std::cout << "JSONIO14 library and API for creating structured data exchange interfaces\n";
            break;
        case 'v':  std::cout << "VelocyPack (VPack) - a fast and compact format for serialization and storage\n";
            break;
        case 'n':  std::cout << "nlohmann/json parser\n";
            break;
        }

        // test all json files into directory
        auto json_files =  jsonio14::files_into_directory( test_dir, ".json" );

        auto start = std::chrono::high_resolution_clock::now();
        switch( parser )
        {
        case 'j':  jsonio14_parse_test( json_files );
            break;
        case 'v':  velocypack_parse_test( json_files );
            break;
        case 'n':  nlohmann_parse_test( json_files );
            break;
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "\nElapsed time in microseconds: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << " μs\n";
    }
    catch(...)
    {
        std::cout <<  "  unknown exception" <<  std::endl;
    }
    return 0;
}

