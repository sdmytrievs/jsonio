#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <vector>
#include <forward_list>

using namespace std;

// https://github.com/dropbox/json11
// https://github.com/nlohmann/json
// https://cristianadam.eu/20151213/qtcreator-and-google-test/
// https://qtcreator.gres.biz/page/gtest/#%D1%81%D0%BA%D0%B0%D1%87%D0%B0%D1%82%D1%8C
// https://doc.qt.io/qtcreator/creator-autotest.html

#include "jsonio/jsonfree.h"
#include "jsonio/jsonschema.h"
#include "jsonio/jsondump.h"
#include "jsonio/service.h"
#include "jsonio/schema_thrift.h"
#include "jsonio/io_settings.h"
#include "tests/example_schema.h"

using namespace jsonio;

/// STL-like access
void AccesstoObjectError();


/// Test read schema
void ReadThriftSchemas()
{
    const SchemasData& all_schemas = ioSettings().Schema();
    std::cout << "Structs: "<< all_schemas.getStructs(false).size() << "\n";
    //for( auto& astruct: all_schemas.getStructs(false) )
    //     std::cout << astruct  << "\n";

//    auto object_limits = JsonSchema::object("LimitsTP");
//    std::cout <<  object_limits << std::endl;
//    object_limits["upperT"] = 200;
//    std::cout <<  object_limits << std::endl;

    auto object = JsonSchema::object("VertexElement");
    std::cout <<  object << std::endl;
    auto json_data = read_ascii_file( "O-old.VertexElement.json" );
    object.loads(json_data);
    std::cout <<  object << std::endl;

    // test read substance to element (add only existing field other skip)
    // json_data = read_ascii_file( "HO2-old.VertexSubstance.json" );
    json_data = read_ascii_file( "methionine.VertexSubstance.json" );
    //std::cout <<  json_data << std::endl;

    auto object_sub = JsonSchema::object("VertexSubstance");
    object_sub.loads(json_data);
    std::cout <<  object_sub << std::endl;
}


int main()
{
    cout << "Hello World!" << endl;
    try{

        ReadThriftSchemas();
        //AccesstoObjectError();
    }
    catch(jsonio_exception& e)
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


/// STL-like access
void AccesstoObjectError()
{
    auto obj =  json::loads( "{\"vbool\":true,\"vint\":-100,\"vdouble\":5.2,\"vstring\":\"Test string\","
                             "\"vlist\":[1.7,2.7,3.7,5.7],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}" );

    // getter/setter
    int  vint;
    std::vector<double> vlist{0,1};
    std::map<std::string, std::string> vumap;
    std::string vstr;

    obj["vint"].get_to(vint);
    obj["vstring"].get_to(vstr);
    obj["vlist"].get_to(vlist);
    obj["vmap"].get_to(vumap);
    //const auto mapdump = obj["vmap"].get<std::string>();

    // not exist field
    std::cout << "obj[\"noexist\"].get_to(vint): " <<  obj["noexist"].get_to(vint)
              << " value is " << vint << std::endl;
    double dd= -101;
    std::cout << "obj[\"noexist\"].get_to(dd): " <<  obj["noexist1"].get_to(dd)
              << " value is " << dd << std::endl;

    auto res =  obj["noexist2"].get_to(vlist);
    std::cout << "obj[\"noexist\"].get_to(vlist): " <<  res << " value is " << json::dump(vlist) << std::endl;

    res = obj["noexist3"].get_to(vstr);
    std::cout << "obj[\"noexist\"].get_to(vstr): " <<  res  << " value is " << vstr << std::endl;

    res = obj["noexist4"].get_to(vumap);
    std::cout << "obj[\"noexist\"].get_to(vumap): " <<  res  << " value is " << json::dump(vumap) << std::endl;


    //std::map<std::string, int> vmap1 = {{std::string("key") , 12}};
    //auto vmap2 = {{std::string("key") , 12}};
}
