#include <iostream>
#include <map>
#include "jsondump.h"
#include "jsonfree.h"
#include "service.h"
using namespace jsonio14;


void CreateanObject()
{
    int  vint{15};
    std::vector<double> vlist{ 17, 27 };
    std::map<std::string, std::string> vumap{ {"key1", "val1" }, {"key2", "val2" } };
    std::string vstr{"New string"};

    auto obj = JsonFree::object();

    obj["vbool"] = true;
    obj["vint"] = vint;
    obj["vdouble"] = 2.5;
    obj["vcstring"] = "const char * string";
    obj["vstr"] =  vstr;
    obj["vlist"] = vlist;
    obj["vmap"] = vumap;
    obj["vmap"]["key3"] = 10;

    std::cout <<  "Created JSON object: \n"  <<  obj <<  std::endl;
}

void Serialization_Deserialization()
{
    // You can create a JSON object (deserialization)
    auto obj =  json::loads( "{ \"happy\": true, \"pi\": 3.141 }" );
    // You can also get a string representation of a JSON value (serialize)
    std::cout <<  " Result: "  <<  json::dump( obj, true) <<  std::endl;
}


int main(int argc, char* argv[])
{
    try{
        CreateanObject();
        Serialization_Deserialization();
    }
    catch(jarango_exception& e)
    {
        std::cout <<   e.what() <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<   "std::exception: " << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  "unknown exception" <<  std::endl;
    }

    return 0;
}

