## Files descriptions ##

1. exeptions.h

General exception structure into project.

> *source: "+";    test: "-";  example: "-"*

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

5. jsonbase.h(cpp)   ( under construction )

A base interface to store JSON object.


> *source: "+-";    test: "+-";  example: "-"*
> Work about

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
> Read NaN and Inf values -> exception

8. jsonfree.h(cpp)

class JsonFree - a class to store JSON object


> *source: "+-";    test: "-";  example: "-"*
> Work about, Need tests, examples

## Next steps

Move dbarango API to other github project

8. json2file.h(cpp)
9. thrift_schema.h(cpp)
10. settings.h(cpp)
11. jsonschema.h(cpp)
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
