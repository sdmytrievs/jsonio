## Files descriptions

1. exeptions.h

General exception structure into project.

> _source: "+";    test: "-";  example: "-"_

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

8. jsonfree.h(cpp)  ( under construction )

class JsonFree - a class to store JSON object


> *source: "+-";    test: "-";  example: "-"*
> Need examples, test special functions ( clear, remove )


To do:

I. test Implementation

```cpp

virtual bool exist_path( const std::string& fieldpath ) const

template <typename T>
bool get_to_path( const std::string& fieldpath, T& val, const T& defval = T(0)  ) const

bool get_to_key( const std::string& fieldpath, std::string& key, const std::string& defkey = "---"  ) const

template <typename T>
bool set_from_path( const std::string& fieldpath, const T& val  )

```


II. Test clear, remove

III. Add functions to resize arrays and get size ( 1D, 2D, 3D .... )

IV. Add iterators ?

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
