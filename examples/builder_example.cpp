#include <iostream>
#include <unordered_map>
#include <list>
#include "jsonio14/jsonbuilder.h"
#include "jsonio14/jsondump.h"
#include "jsonio14/jsonfree.h"
using namespace jsonio14;
using namespace std;

void buildArray()
{
    auto jsFree =JsonFree::array();
    JsonArrayBuilder jsBuilder(&jsFree);

    double val=0.5;
    for( int ii=0; ii<5; ++ii )
        jsBuilder.addValue( val*ii );

    jsFree.dump(std::cout, false);
}


void buildOneObject()
{
    int  vint = 3;
    float vfloat = 2.5;
    double vdouble = 1e-10;
    std::list<double> vlist = { 1.7, 2.7, 3.7, 4.7 };
    std::unordered_map<std::string, std::string> vumaps = { {"key1", "val1" }, {"key2", "val2" } };

    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addNull( "vnull").addBool( "vbool", true ).addInt( "vint", vint );
    jsBuilder.addDouble( "vdouble", vdouble ).addString( "vstring", "Test String" );
    jsBuilder.addValue( "vfloat", vfloat );
    jsBuilder.addVector( "vlist", vlist );
    jsBuilder.addMapKey( "vumap", vumaps );

    jsFree.dump(std::cout, false);
}

void buildComplexObject()
{
    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addString( "name", "ComplexObject" );

    auto arr = jsBuilder.addArray("array");
    for( int ii=0; ii<10; ++ii )
    {
        arr.addValue(ii);
    }

    auto obj = jsBuilder.addObject("object").addInt("version", 1).addString("comment", "Test example");

    std::cout << jsFree;
}

/// Infers the value type by parsing input string.
void buildObjectInfers()
{
    auto jsFree =JsonFree::object();
    JsonObjectBuilder jsBuilder(&jsFree);

    jsBuilder.addScalar( "vnull", "null").addScalar( "vbool1", "true" )
            .addScalar( "vbool2", "false" ).addScalar( "vint", "3" )
            .addScalar( "vdouble", "1e-10" ).addScalar( "vstring", "Test String" );

    jsFree.dump(std::cout, false );
}

int main()
{
    try{
        buildArray();
        buildOneObject();
        buildComplexObject();
        buildObjectInfers();
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
