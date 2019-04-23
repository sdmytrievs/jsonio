#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <vector>

using namespace std;

// https://github.com/dropbox/json11
// https://github.com/nlohmann/json
// https://cristianadam.eu/20151213/qtcreator-and-google-test/
//https://qtcreator.gres.biz/page/gtest/#%D1%81%D0%BA%D0%B0%D1%87%D0%B0%D1%82%D1%8C
// https://doc.qt.io/qtcreator/creator-autotest.html


#include "jsonbase.h"
#include "jsonfree.h"
using namespace jsonio14;

void testDumps()
{
    string str("Test string.");


    std::cout << v2string(true) << std::endl;
    std::cout << v2string(100) << std::endl;
    std::cout << v2string(1e-5) << " " << v2string(5.6f) << std::endl;
    std::cout << v2string("Test char *") << std::endl;
    std::cout << v2string(str) << std::endl;
    std::cout << v2string('n') << std::endl;

    uint uval = 100;
    unsigned char ch = 'Q';
    std::cout << v2string(uval) << std::endl;
    std::cout << v2string(ch) << std::endl;

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    std::map<std::string, bool> vmapb = { {"key1", true }, {"key2", false } };
    std::map<std::string, std::string> vmaps = { {"key1", "val4" }, {"key2", "val5" } };

    std::cout << dump(vmap) << std::endl;
    std::cout << dump(vmapb) << std::endl;
    std::cout << dump(vmaps) << std::endl;

    std::vector<int> vvec = { 1, 2, 3, 4};
    std::vector<double> vvecb = { 1.5, 2.5, 3.5, 4.5 };
    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };

    std::cout << dump(vvec) << std::endl;
    std::cout << dump(vvecb) << std::endl;
    std::cout << dump(vvecs) << std::endl;

    std::set<double> vset = { 1.6, 2.6, 3.6, 4.6 };
    std::list<double> vlist = { 1.7, 2.7, 3.7, 4.7 };

    std::cout << dump(vset) << std::endl;
    std::cout << dump(vlist) << std::endl;

    std::vector<std::vector<std::string>> vvvec;

    for (int ii=0; ii<5; ii++) {
       std::vector<std::string> vin;
      for (int jj=0; jj<3; jj++)
       vin.push_back( std::to_string(ii)+"\"_\""+ std::to_string(jj));
      vvvec.push_back(vin);
    }
    std::cout << dump(vvvec) << std::endl;
    //std::cout << dump(vvvec) << std::endl;

    std::cout << "Test is_container" << std::endl;
    std::cout << is_container<std::vector<int>>::value << std::endl; //true
    std::cout << is_container<std::list<int>>::value << std::endl;   //true
    std::cout << is_container<std::map<std::string, int>>::value << std::endl;    //true
    std::cout << is_container<std::set<int>>::value << std::endl;    //true
    std::cout << is_container<int>::value << std::endl;              //false

    std::cout << "Test is_mappish" << std::endl;
    std::cout << is_mappish<std::vector<int>>::value << std::endl; //false
    std::cout << is_mappish<std::list<int>>::value << std::endl;   //false
    std::cout << is_mappish<std::map<std::string, int>>::value << std::endl;    //true
    std::cout << is_mappish<std::multimap<std::string, double>>::value << std::endl;    //false
    std::cout << is_mappish<std::unordered_map<std::string, int>>::value << std::endl;    //true
    std::cout << is_mappish<std::unordered_multimap<std::string, int>>::value << std::endl;    //false
    std::cout << is_mappish<std::set<int>>::value << std::endl;    //false
    std::cout << is_mappish<int>::value << std::endl;              //false

}


void TestBaseType()
{
    auto jsFree =JsonFree::object();

    bool vbool = true;

    char vchar = 'a';
    int  vint  = -1;
    long vlong = 2l;
    unsigned int  vuint = 3;
    long long vlonglong = 4L;

    float vfloat = 2.5;
    double vdouble = 1e-10;

    std::vector<std::string> vvecs = { "v1", "v2", "v3", "v4" };
    std::set<int> vset = { 1, 2, 6, 4 };
    std::list<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::vector<int> vvec = { 1, 2, 3, 4};

    std::map<std::string, int> vmap = { {"key1", 4 }, {"key2", 5 } };
    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };
    string vstr("Test string.");


    jsFree[ std::string("set") ] = vset;
    jsFree[ std::string("vector") ].setArray( vvec );
    vvec.clear();
    jsFree[ std::string("vector") ].getArray( vvec );


    for( auto& el: vvec )
     std::cout <<  el << "  " ;
    std::cout << endl;

    int mTndx = 10;
    std::string mT="bbbbbb";
    std::map<std::string, int> vmap1 = {{std::to_string(mTndx) , 12}};
    //auto vmap1 = {{std::to_string(mTndx) , mT}};
//    jsFree["method"] = vmap1;
    jsFree["vlonglong"] = vlonglong;


    /*
    std::cout << "Type bool " << jsFree.typeTraits( vbool ) <<  std::endl;
    std::cout << "Type char " << jsFree.typeTraits( vchar ) <<  std::endl;
    std::cout << "Type int " << jsFree.typeTraits( vint ) <<  std::endl;
    std::cout << "Type long " << jsFree.typeTraits( vlong ) <<  std::endl;
    std::cout << "Type u_int " << jsFree.typeTraits( vuint ) <<  std::endl;
    std::cout << "Type long long " << jsFree.typeTraits( vlonglong ) <<  std::endl;
    std::cout << "Type float " << jsFree.typeTraits( vfloat ) <<  std::endl;
    std::cout << "Type double " << jsFree.typeTraits( vdouble ) <<  std::endl;
    std::cout << "Type std::vector<std::string> " << jsFree.typeTraits( vvecs ) <<  std::endl;
    std::cout << "Type std::set<int> " << jsFree.typeTraits( vset ) <<  std::endl;
    std::cout << "Type std::list<double> " << jsFree.typeTraits( vlist ) <<  std::endl;
    std::cout << "Type std::map<std::string, int> " << jsFree.typeTraits( vmap ) <<  std::endl;
    std::cout << "Type std::unordered_map<std::string, std::string> " << jsFree.typeTraits( vumaps ) <<  std::endl;
    std::cout << "Type nullptr " << jsFree.typeTraits( nullptr ) <<  std::endl;
    std::cout << "Type string " << jsFree.typeTraits( vstr ) <<  std::endl;
    std::cout << "Type const char * " << jsFree.typeTraits( "Test char *" ) <<  std::endl;
*/
}


int main()
{
    testDumps();
    //TestBaseType();
    cout << "Hello World!" << endl;
    return 0;
}
