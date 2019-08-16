#include <iostream>
#include <unordered_map>
#include <list>

#include "jsonbuilder.h"
#include "jsondump.h"
#include "jsonfree.h"
using namespace jsonio14;
using namespace std;

void buildArray()
{
    auto jsFree =JsonFree::array();
    JsonArrayBuilder jsBuilder(jsFree);

    double val=0.5;
    for( int ii=0; ii<5; ++ii )
        jsBuilder.addValue( val*ii );

    json::dump(std::cout, jsFree, false);
}


void buildOneObject()
{
    int  vint = 3;
    float vfloat = 2.5;
    double vdouble = 1e-10;
    std::list<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val1" }, {"key2", "val2" } };

    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(jsFree);

    jsBuilder.addNull( "vnull").addBool( "vbool", true ).addInt( "vint", vint );
    jsBuilder.addDouble( "vdouble", vdouble ).addString( "vstring", "Test String" );
    jsBuilder.addValue( "vfloat", vfloat );
    jsBuilder.addVector( "vlist", vlist );
    jsBuilder.addMapKey( "vumap", vumaps );

    json::dump(std::cout, jsFree, false);
}

void buildComplexObject()
{
    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(jsFree);

    jsBuilder.addString( "name", "ComplexObject" );

    auto arr = jsBuilder.addArray("array");
    for( int ii=0; ii<10; ++ii )
    {
        arr.addValue(ii);
    }

    auto obj = jsBuilder.addObject("object").addInt("version", 1).addString("comment", "Test example");

    std::cout << json::dump( jsFree, false );
}

/// Infers the value type by parsing input string.
void buildObjectInfers()
{
    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(jsFree);

    jsBuilder.addScalar( "vnull", "null").addScalar( "vbool1", "true" )
            .addScalar( "vbool2", "false" ).addScalar( "vint", "3" )
            .addScalar( "vdouble", "1e-10" ).addScalar( "vstring", "Test String" );

    json::dump(std::cout, jsFree, false );
}

int main()
{
    try{
        buildArray();
        buildOneObject();
        buildComplexObject();
        buildObjectInfers();
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
