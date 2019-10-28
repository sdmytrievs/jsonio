## Files descriptions

1. exeptions.h

General exception structure into project.

> _source: "+";    test: "-";  example: "+"_

2. service.h(cpp)

    - Working with regular expressions
    - Trim characters functions
    - Get all regular file names from the directory
    - Read whole ASCII file into string
    - Comparing floating point
    - Other best practices

> *source: "+";    test: "+";  example: "-"*

3. jsondetail.h(cpp)  + type_test.h

    - Type check macros  ( is_container, is_mappish )
    - Basic functions for read/write numeric values to/from string
    - Split string to query functions

> *source: "+";    test: "+";  example: "-"*

4. jsondump.h(cpp)

    - Dump escaped string (json::dump)
    - Serialize an integral type value,  vector-like objects and  map-like objects to a json string (json::dump)
    - Serialize a JsonBase object as a JSON formatted stream (json::dump)
    - Deserialize a JSON string to a object (json::loads)

```cpp

/// @brief Dump object to JSON string.
std::string dump( const JsonBase& object, bool dense = false );
/// Serialize object as a JSON formatted stream.
void dump( std::ostream& os, const JsonBase& object, bool dense = false );
/// Deserialize a JSON document to a free format json object.
JsonFree loads( const std::string& jsonstr );
/// Deserialize a JSON document to a schema define json object.
JsonSchema loads( const std::string& schema_name, const std::string& jsonstr );

```

> *source: "+";    test: "+-";  example: "-"*
> Could be more tests about dump/undump strings

5. jsonbase.h(cpp)

A base interface to store JSON object.


> *source: "+-";    test: "+";  example: "-"*
> Added typed test to most virtual functions

6. jsonbuilder.h(cpp)

    - class JsonObjectBuilder - builder for creating JsonBase::Object models from scratch
    - class JsonArrayBuilder -  builder for creating JsonBase::Array from scratch

```cpp

auto jsFree =JsonFree::object();
JsonObjectBuilder jsBuilder(jsFree);

jsBuilder.addNull( "vnull").addBool( "vbool", true ).addInt( "vint", 3 )
         .addDouble( "vdouble", 1e-10 ).addString( "vstring", "Test String" );

json::dump(std::cout, jsFree, false );

```

> *source: "+";    test: "+";  example: "+"*

7. jsonparser.h(cpp)

class JsonParser - read JsonBase structure from json string.

```cpp

JsonFree loads( const std::string &jsonstr )
{
    auto object = JsonFree::object();
    JsonParser parser(jsonstr);
    parser.parse_to(object);
    return object;
}

```
> *source: "+";  test: "+";  example: "+"*
> Read NaN and Inf values -> exception?

8. jsonfree.h(cpp)

class JsonFree - a class to store free schema JSON object

```cpp

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


for (JsonFree::iterator it = obj.begin(); it != obj.end(); ++it) {
  std::cout << it->toString(true) << '\n';
}


```

> *source: "+";    test: "+";  example: "+"*
> Error when move assignment: object["key"]= std::move(anotherobject);
> key would be overwritten by anotherobject key

9. txt2file.h(cpp)

Functions and classes to work with text files and directories.
Classes for read/write json objects and json arrays files.
Subset of the Filesystem library provides facilities for performing operations on file systems and their components, such as paths, regular files, and directories.


> *source: "+";    test: "+";  example: "-"*


10. io_settings.h(cpp)  ( could be new functions in future )

class JsonioSettings - storing preferences to JSONIO


> *source: "+-";    test: "+";  example: "-"*


To do:

- add time compare tests json parsers

V. New API working with schemas

 a. We create Interfaces   StructDef and FieldDef  for working with schema descriptions
 b. Then public classes ThriftStructDef and ThiftFieldDef implementation
 c. In future we can have JsonStructDef and JsonFieldDef implementation [JsonShema](http://json-schema.org/)
 d. Functions getVertexList, getEdgesList, getVertexCollection, getEdgeCollection, getVertexName could be moved to database implementation and add functions for set this data. This data could be setted when read thrift scheas or other place.
 e. Into SchemaJson get links to base StructDef and FieldDef and implement ```c++ template<type T> static SchemaJson::object( const string& schema_name ) ```
 f. Into  SchemaJson use ```c++ const  StructDef* const;   const  FieldDef* const  ``` for only read data.
 g. Into  StructDef functions: getName(), testUnion(), getOtherStruct( struct_name ), getField( field_name ).
 h. Into FieldDef add get functions for all fields.

## Next steps

Move dbarango API to other github project

9. thrift_schema.h(cpp)
11. jsonschema.h(cpp)

## Database

12. dbquerydef.h(cpp)
13. dbconnect.h(cpp)
14. dbarango.h(cpp)
15. dbcollection.h(cpp)
16. dbdocument.h(cpp)
17. dbjsondoc.h(cpp)
18. dbschemadoc.h(cpp)
19. dbvertexdoc.h(cpp)
20. dbedgedoc.h(cpp)
21. traversal.h(cpp)

Work about jsonimpex
Work about jsonui

## Best practices

// Top 25 C++ API design mistakes and how to avoid them
// https://www.acodersjourney.com/top-25-cplusplus-api-design-mistakes-and-how-to-avoid-them/

// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
// If you need global access to an object, make it a global, like std::cout. But don't constrain the number of instances that can be created.


