#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <vector>

#include "jsonbuilder.h"
#include "jsonfree.h"
using namespace jsonio14;
using namespace std;



void buildOneObject()
{
    unsigned int  vuint = 3;
    float vfloat = 2.5;
    double vdouble = 1e-10;
    std::list<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val4" }, {"key2", "val5" } };


    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(jsFree);

    jsBuilder.addNull( "vnull").addBool( "vbool", true ).addInt( "vuint", vuint );
    jsBuilder.addDouble( "vdouble", vdouble ).addString( "vstring", "Test String" ).addScalar( "scalar", "55" );
    jsBuilder.addValue( "vfloat", vfloat );
    jsBuilder.addVector( "vlist", vlist );
    jsBuilder.addMapKey( "vumaps", vumaps );

    //JsonFree jsNew = jsBuilder;
    std::cout << json::dump( jsFree, false);
}

void buildComplexObject()
{
    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(jsFree);

    jsBuilder.addNull( "vnull").addBool( "vbool", true );

    auto arr = jsBuilder.addArray("array");
    for( int ii=0; ii<10; ++ii )
    {
        arr.addValue(ii);
    }
    auto obj = jsBuilder.addObject("object").addInt("obj1", 1).addString("obj2", "Test");
    std::cout << json::dump( jsFree, true);
}


int main()
{
    try{

        buildOneObject();
        buildComplexObject();

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
