## JSONIO17 


Jsonio17 library and API for creating structured data exchange interfaces (in RAM, via files, and using ArangoDB remote and local instances). 
The data munging is based on JSON schemas connected with the internal JSONDOM object. A JSON schema can be easily generated from a Thrift data structure definition (.thrift file) for a given structured data type.


### What Jsonio17 does? 


* Jsonio17 implements the rendering to/from file or text stream into ArangoDB and/or JSON format. 
* Jsonio17 can also be used for connecting any user-defined C++ data structures to a GUI editor widget and graphics (using the JSONUI17 widget API).
* JSONIO is written in C/C++ using open-source library Velocypack from ArangoDB.
* Version: currently 0.1.
* Will be distributed as is (no liability) under the terms of Lesser GPL v.3 license. 

### Why use Jsonio17? 

Advantages of the Jsonio17 JSON parser:

* Full support of JSON schemas (the same API can be used for schemaless and for schema-related JSON documents).
* Fixed order of JSON object keys in the document (convenient for rendering in GUIs and comparing dump JSON output).
* Comprises the basis of JSONUI17 GUI widgets for editing and viewing schema-related and schemaless JSON documents kept in NoSQL databases such as ArangoDB.


### How to get Jsonio17 source code

* In your home directory, make a folder named e.g. ~/jsonio17.
* cd ~/jsonio17 and clone this repository from https://bitbucket.org/gems4/jsonio17.git  using a preinstalled free git client SourceTree or SmartGit (the best way on Windows). 
* Alternatively on Mac OS X or linux, open a terminal and type in the command line (do not forget a period):

```sh
git clone https://bitbucket.org/gems4/jsonio17.git . 

```

### How to install the Jsonio17 library  Ubuntu and MacOS

 Building Jsonio17 requires g++, [CMake](http://www.cmake.org/), Velocypack, jsonArango.

* Make sure you have g++, cmake and git installed. If not, install them. 

  On Ubuntu linux terminal, this can be done using the following commands:

```sh
#!bash
sudo apt-get install g++ cmake git libssl-dev libtool byacc flex
```

  For Mac OSX, make sure you have Homebrew installed (see [Homebrew web site](http://brew.sh) and [Homebrew on Mac OSX El Capitan](http://digitizor.com/install-homebrew-osx-el-capitan/) ).


* Install Dependencies

```sh
#!bash
cd ~/jsonio17
sudo ./install-dependencies.sh
```

* Install the Jsonio17 library

Then navigate to the directory where this README.md file is located and type in terminal:

```sh
cd ~/jsonio17
sudo ./install.sh
```

* After that, headers, library  and the third-party libraries can be found in /usr/local/{include,lib}. 

* Install current version of ArangoDB server locally ( see [jsonArango](https://bitbucket.org/gems4/jsonarango/src/master/) ).


### How to use Jsonio17 (use cases) 

* Files parse example

```c++

int main(int argc, char* argv[])
{
    string test_dir = ".";
    if( argc > 1)
        test_dir = argv[1];

    try{

        // test all json files into directory
        auto fail_json_files =  files_into_directory( test_dir, ".json" );
        for( auto file: fail_json_files)
        {
            try{
                auto json_data = read_ascii_file( file );
                auto jsFree =  json::loads( json_data );
                std::cout <<  "Pass: " <<  file <<  std::endl;
            }
            catch( jsonio_exception& e )
            {
                std::cout << "Fail: " << file <<  "  Exeption: "  <<  e.what() <<  std::endl;
            }
            catch( std::exception& e )
            {
                std::cout << "Fail: " << file << " std::exception: " << e.what() <<  std::endl;
            }
        }
    }
    catch(...)
    {
        std::cout <<  "  unknown exception" <<  std::endl;
    }

    return 0;
}

```

* Example the API for manipulating collections and documents into.

```c++ 

int main(int, char* [])
{
    JsonioSettings::settingsFileName = "jsonio17-config.json";
    // Test collection name
    std::string collectionName = "test";
    int numOfDocuments =  10;

    try{

        // Connect to Arangodb ( load settings from "jsonio17-config.json" config file )
        DataBase db;
        // Open collection, if document collection collectionName not exist it would be created
        auto coll= db.collection( collectionName );

        // Insert documents to database
        for( int ii=0; ii<numOfDocuments; ii++ )
        {
            auto jsFree = JsonFree::object();
            jsFree["name"] = (  ii%2 ? "a" : "b" );
            jsFree["index"] = ii;
            jsFree["task"] = "exampleQuery";
            jsFree["properties"]["value"] = 10.01*ii;
            auto rkey = coll->createDocument( jsFree );
            recKeys.push_back(rkey);
        }

        // Define call back function
        SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
        {
            std::cout <<  jsondata <<  std::endl;
        };

         // Select records by AQL query
        recjsonValues.clear();
        std::string aql = "FOR u IN " + collectionName +
                "\nFILTER u.properties.value > 50 \n"
                "RETURN { \"_id\": u._id, \"name\":u.name, \"index\":u.index }";
        DBQueryBase    aqlquery( aql, DBQueryBase::qAQL );
        coll->selectQuery( aqlquery, setfnc );
 
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

```

For more information on APIs and more examples, take a look into the /examples directory.

#### [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html) documentation

Install doxygen

```sh
sudo apt install doxygen
sudo apt install graphviz
```

Generate html help

```sh
doxygen  jsonio-doxygen.conf
```

#### How to generate JSON schemas using thrift

Use the following command:

```sh
thrift -r --gen json schema.thrift
```

The option -r (recursively) is necessary for generating json files from potential includes in the schema.thrift file. 


