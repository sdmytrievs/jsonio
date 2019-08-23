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
//https://qtcreator.gres.biz/page/gtest/#%D1%81%D0%BA%D0%B0%D1%87%D0%B0%D1%82%D1%8C
// https://doc.qt.io/qtcreator/creator-autotest.html

#include "jsonfree.h"
#include "jsondump.h"
using namespace jsonio14;

/// STL-like access
void AccesstoObjectError()
{
    auto obj =  json::loads( "{\"vbool\":true,\"vint\":-100,\"vdouble\":5.2,\"vstring\":\"Test string\","
                             "\"vlist\":[1.7,2.7,3.7,5.7],\"vmap\":{\"key1\":\"val1\",\"key2\":\"val2\"}}" );

    // getter/setter
    int  vint;
    std::vector<double> vlist;
    std::map<std::string, std::string> vumap;
    std::string vstr;

    bool vbool = obj["vbool"].get<bool>();
    auto vint2 = obj["vint"].get<int>();
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

     std::cout << "obj[\"noexist\"].get_to(vlist): " <<  obj["noexist2"].get_to(vlist)
               << " value is " << json::dump(vlist) << std::endl;

     std::cout << "obj[\"noexist\"].get_to(vstr): " <<  obj["noexist3"].get_to(vstr)
               << " value is " << vstr << std::endl;

    //std::map<std::string, int> vmap1 = {{std::string("key") , 12}};
    //auto vmap2 = {{std::string("key") , 12}};
}

int main()
{
    cout << "Hello World!" << endl;
    try{
        AccesstoObjectError();
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


