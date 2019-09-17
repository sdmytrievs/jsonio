#include <iostream>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "jsondump.h"
#include "jsonfree.h"
#include "service.h"
using namespace jsonio14;

/// Create object example
void CreateanObject();
///
void SerializationDeserialization();

/// STL-like access
void AccesstoObject();
/// Conversion from STL containers
void STLcontainers();

void FieldPathMethods();

int main(int , char** )
{
    try{
        CreateanObject();
        //SerializationDeserialization();
        //AccesstoObject();
        //STLcontainers();
        //FieldPathMethods();
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

    // iterate the array
    for (JsonFree::iterator it = obj.begin(); it != obj.end(); ++it) {
      std::cout << it->toString(true) << '\n';
    }

    // range-based for
    for (const auto& element : obj["vlist"]) {
      std::cout << element << '\n';
    }

    // range-based for (empty)
    for (const auto& element : obj["vdouble"]) {
      std::cout << element << '\n';
    }
}

void SerializationDeserialization()
{
    // You can create a JSON object (deserialization)
    auto obj =  json::loads( "{ \"happy\": true, \"pi\": 3.141 }" );
    // You can also get a string representation of a JSON value (serialize)
    std::cout <<  " Result: "  <<  obj.dump(true) <<  std::endl;
}

/// STL-like access
void AccesstoObject()
{
    auto obj =  json::loads( "{\"vbool\":true,\"vint\":-100,\"vdouble\":5.2,\"vstring\":\"Test string\","
                             "\"vlist\":[1.7,2.7,3.7,5.7],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}" );

    // Convenience type checkers
    obj.isTop();
    obj["vbool"].isNull();
    obj["vbool"].isBool();
    obj["vdouble"].isNumber();
    obj["vmap"].isObject();
    obj["vlist"].isArray();
    obj["vdouble"].isString();

    // getter/setter
    int  vint;
    std::vector<double> vlist;
    std::map<std::string, std::string> vumap;
    std::string vstr;

    obj["vint"].get_to(vint);
    obj["vstring"].get_to(vstr);
    obj["vlist"].get_to(vlist);
    obj["vmap"].get_to(vumap);

    // Other stuff
    obj.size();
    obj.empty();
    obj.type();
    obj["vlist"].clear();
}

// Conversion from STL containers
void STLcontainers()
{
    auto jsFree = JsonFree::object();

    std::vector<std::string> vvec2, vvec = { "v1", "v2", "v3", "v4" };
    jsFree.set_from(vvec);
    std::cout<< "vector\n" << jsFree.dump(  true );
    jsFree.get_to(vvec2);
    std::cout<< json::dump( vvec2 ) << std::endl;

    std::set<int> vset = { 1, 2, 6, 4 };
    jsFree.set_from(vset);
    std::cout<< "set\n" << jsFree.dump(  true );

    std::list<double> vlist2, vlist = { 1.7, 2.7, 4.7, 3.7, .5 };
    jsFree.set_list_from(vlist);
    std::cout<< "list\n" << jsFree.dump(  true );
    jsFree.get_to_list(vlist2);
    std::cout<< json::dump( vlist2 ) << std::endl;

    std::forward_list<int> fwd_list= {11,12,13,14,15};
    jsFree.set_from(fwd_list);
    std::cout<< "forward_list\n" << jsFree.dump(  true );

    std::unordered_set<int> uset = {1, 3, 5, 4, 5};
    jsFree.set_list_from(uset);
    std::cout<< "unordered_set\n" << jsFree.dump(  true );

    std::multiset<int> mset = {1, 3, 5, 4, 5};
    jsFree.set_from(mset);
    std::cout<< "multiset\n" << jsFree.dump(  true ) << std::endl;

    // ----------------------------------------------------------

    std::map<std::string, int> vmap2, vmap = { {"key1", 4 }, {"key2", 5 } };
    jsFree.set_from(vmap);
    std::cout<< "map\n" << jsFree.dump(  true );
    jsFree.get_to(vmap2);
    std::cout<< json::dump( vmap2 ) << std::endl;

    std::unordered_map<std::string, std::string> vumap2, vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    jsFree.set_map_from(vumaps);
    std::cout<< "unordered_map\n" << jsFree.dump(  true );
    jsFree.get_to_map(vumap2);
    std::cout<< json::dump( vumap2 ) << std::endl;
}

void FieldPathMethods()
{
    std::string input_json = "{\"about\":{\"version\":1,\"description\":\"About\"},\"formats\":"
                                       "{\"int\":{\"width\":5,\"precision\":0},\"float\":{\"width\":10,\"precision\":4},"
                                       "\"double\":{\"width\":15,\"precision\":6}},\"data\":[{\"group\":\"float\",\"value\":1.4},"
                                       "{\"group\":\"int\",\"value\":100},{\"group\":\"double\",\"value\":1e-10},{\"group\":\"double\",\"value\":1e10}],"
                                       "\"values\":[[1,2,3],[11,12,13]]}";

    auto obj =  json::loads( input_json );
    auto path = obj["about"]["description"].get_path();
    std::cout << path << std::endl;

}
