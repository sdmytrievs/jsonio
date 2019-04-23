#pragma once


#include "jsonfree.h"

namespace jsonio14 {


//template <class T>
class JsonParser final
{

public:

    explicit JsonParser()
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
