## Files descriptions

### Json and schema json parsing ###

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
  std::cout << it->get()->toString(true) << '\n';
}

```

> *source: "+";    test: "+";  example: "+"*

9. txt2file.h(cpp)

Functions and classes to work with text files and directories.
Classes for read/write json objects and json arrays files.
Subset of the Filesystem library provides facilities for performing operations on file systems and their components, such as paths, regular files, and directories.


> *source: "+";    test: "+";  example: "-"*


10. io_settings.h(cpp)

class JsonioSettings - storing preferences to JSONIO


> *source: "+-";    test: "+";  example: "-"*


11. schema.h

New API working with schemas. We create Interfaces  for working with schema descriptions
Interfaces FieldDef - Definition of one field in thrift structure;
           StructDef - Structured data definition;
           EnumDef - Enum data definition

Class SchemasData - all json schemas collection

> *source: "+";    test: "+";  example: "-"*
> d. Functions getVertexList, getEdgesList, getVertexCollection, getEdgeCollection, getVertexName could be moved to database implementation and add functions for set this data. This data could be setted when read thrift scheas or other place.
> e. Into SchemaJson get links to base StructDef and FieldDef and implement ```c++ template<type T> static SchemaJson::object( const string& schema_name ) ```
> f. Into  SchemaJson use ```c++ const  StructDef* const;   const  FieldDef* const  ``` for only read data.
> g. Into  StructDef functions: getName(), testUnion(), getOtherStruct( struct_name ), getField( field_name ).


12. schema_thrift.h(cpp)

Implementation of FieldDef, StructDef, EnumDef interfaces for Thrift schemas

> *source: "+";    test: "+";  example: "-"*

13. schema_json.h(cpp)   - not implemented

To be done
In future we can have JsonStructDef and JsonFieldDef implementation [JsonShema](http://json-schema.org/)

> *source: "-";    test: "-";  example: "-"*

14. jsonschema.h(cpp)

class JsonSchema - a class to store schema based JSON object

```cpp

int  vint{15};
std::vector<double> vlist{ 17, 27 };
std::map<std::string, std::string> vumap{ {"key1", "val1" }, {"key2", "val2" } };
std::string vstr{"New string"};

auto simple_object = JsonSchema( JsonSchema::object("SimpleSchemaTest") );

simple_object["vbool"] = true;
simple_object["vint"] = vint;
simple_object["vdouble"] = 2.5;
simple_object["vstring"] =  vstr;
simple_object["vlist"] = vlist;
simple_object["vlist"][2] = 11;
simple_object["vmap"] = vumap;
simple_object["vmap"]["key3"] = "10";

std::cout <<  simple_object << std::endl;

```


> By default add requered fields and fields with default value
> *source: "+";    test: "+";  example: "+"*

### Working with database ###


15. dbquerybase.h(cpp)

class DBQueryBase describing the query and query parameters. Used to retrieve data that are stored in the DataBase.
class DBQueryDef is a query description into a Database record.
class DBQueryResult used to store query definition and result.

> *source: "+";  test: "+-";  example: "-"*
> No tests for DBQueryResult

16. dbdriverbase.h

Interface for Abstract Database Driver
The driver works like an adaptor which connects a generic interface to a specific database vendor implementation.

> *source: "+";  test: "-";  example: "-"*

17. dbdriverarango.h(cpp)

Implementation of Database Driver using Low-Level C++ Driver for ArangoDB.

> *source: "+";  test: "-";  example: "-"*
> All tests into jsonArango

18. dbconnect.h(cpp)

class DataBase to managing database connection (thread-safee).

> *source: "+";  test: "-";  example: "+"*
> ? tests


19. dbcollection.h(cpp)

class  DBCollection  the definition collections API (thread-safee).  A collection contains zero or more documents.
Function loadCollectionFile runs in the other thread.

> *source: "+";  test: "-";  example: "+"*
> ? tests

20. dbdocument.h(cpp)

Implementation interface DBDocumentBase for working with documents. Documents are JSON like objects.
These objects can be nested (to any depth) and may contain lists.  Each document has a unique primary key that identifies it within its collection.
Furthermore, each document is uniquely identified by its document handle across all collections in the same database.

> *source: "+";  test: "-";  example: "-"*
> Implement some addition functions
> ? tests


21. dbjsondoc.h(cpp)

class DBJsonDocument - implementation of the database document  as free structure JSON

> *source: "+";  test: "-";  example: "-"*
> ? tests

22. dbschemadoc.h(cpp)

class DBSchemaDocument - implementation of the database document as schema-based JSON

> *source: "+";  test: "-";  example: "-"*
> ? tests


23.  dbvertexdoc.h(cpp)

Definition of graph databases chain: Handling Vertices
class DBVertexDocument - implementation of the database vertex as schema-based JSON.

> *source: "+";  test: "-";  example: "-"*
> Handling  unique fields values are blocked
> ? tests

24.  dbedgedoc.h(cpp)

Definition of graph databases chain: Handling Edges
class DBEdgeDocument - implementation of the database edge as schema-based JSON.

> *source: "+";  test: "-";  example: "-"*
> ? tests


25. traversal.h(cpp)

class GraphTraversal implementation traversal of graph  for Database

> *source: "+";  test: "-";  example: "-"*
> ? tests


## Next steps --------------------------------------------------------------------------------

To do:

- Implement examples and tests for database part ( try use googlemock )
http://artlang.net/post/unit-testirovaniye-c++-mock-obyekty-google-c++-mocking-framework/

- add top level std::shared_ptr

## Important links  --------------------------------------------------------------------------------


https://gist.github.com/Cartexius/4c437c084d6e388288201aadf9c8cdd5

sudo apt-get update -y
sudo apt-get install -y googletest



## Best practices

// Top 25 C++ API design mistakes and how to avoid them
// https://www.acodersjourney.com/top-25-cplusplus-api-design-mistakes-and-how-to-avoid-them/

// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
// If you need global access to an object, make it a global, like std::cout. But don't constrain the number of instances that can be created.

// http://www.vishalchovatiya.com/21-new-features-of-modern-cpp-to-use-in-your-project/
// 21 new features of Modern C++ to use in your project

- tried change pointer to "optional" value - not works for reference values.
https://en.cppreference.com/w/cpp/utility/optional
