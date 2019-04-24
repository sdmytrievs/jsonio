#pragma once

#include "jsonbuilder.h"

namespace jsonio14 {


/// Class for read/write json structure from/to text file or string
class JsonParser
{
  protected:

  std::string jsontext;
  const char *curjson;
  const char *end;

  bool xblanc();
  void parse_string( std::string& str );
  void parse_number( double& value, int& type );
  void parse_value( const std::string& name, JsonObjectBuilder& builder );
  //void print_stream( ostream& os, const JsonDom* object, int depth, bool dense );

public:

  explicit JsonParser():curjson(nullptr), end(nullptr)
  { }


  /// Parse internal jsontext string to bson structure (without first {)
  void parse_object( JsonObjectBuilder& builder );
  void parse_array( JsonArrayBuilder& builder );

};



//template <class T>
class JsonParser1 final
{

public:

    explicit JsonParser1()
    { }

    // Serialize json object to string.
    static void dump(std::string& out, const JsonBase& object, int dense = 0);


    static std::string dump(const JsonBase& object, int dense = 0)
    {
      std::string out;
      dump(out, object, dense);
      return out;
    }


    /// Parse one Json object from string
    bool parse( const std::string& json, JsonBase& object );


};


} // namespace jsonio14
