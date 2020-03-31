#include <iostream>
#include "jsonio14/jsonschema.h"
#include "jsonio14/jsondump.h"
#include "jsonio14/schema_thrift.h"
#include "jsonio14/io_settings.h"
#include "tests/example_schema.h"

using namespace std;
using namespace jsonio14;

void SimpleSchema()
{
    int  vint{15};
    std::vector<double> vlist{ 17, 27 };
    std::map<std::string, std::string> vumap{ {"key1", "val1" }, {"key2", "val2" } };
    std::string vstr{"New string"};

    auto simple_object = JsonSchema( JsonSchema::object("SimpleSchemaTest") );
    //std::cout <<  simple_object.dump(true) << std::endl;

    simple_object["vbool"] = true;
    simple_object["vint"] = vint;
    simple_object["vdouble"] = 2.5;   // order test
    simple_object["vstring"] =  vstr;
    simple_object["vlist"] = vlist;
    simple_object["vlist"][2] = 11;
    simple_object["vmap"] = vumap;
    simple_object["vmap"]["key3"] = "10";

    std::cout <<  simple_object << std::endl;
}


void ComplexSchema()
{
    auto complex_object = JsonSchema( JsonSchema::object("ComplexSchemaTest") );
    std::cout <<  complex_object.dump(true) << std::endl;

    auto describe_object = JsonSchema( JsonSchema::object("Describe") );
    describe_object["description"] = "the description";

    auto spdata_object = JsonSchema( JsonSchema::object("SpecifiersData") );
    spdata_object["group"] = "the group";

    auto format_object = JsonSchema( JsonSchema::object("FormatData") );
    format_object["width"] = 10;
    format_object["precision"] = 8;

    complex_object["about"] = describe_object;
    complex_object["formats"]["new"] = format_object;
    complex_object["data"][0] =spdata_object;

    std::cout <<  complex_object << std::endl;
}


int main()
{
    try{
        ioSettings().addSchemaFormat(schema_thrift, schema_str);
        SimpleSchema();
        ComplexSchema();
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

