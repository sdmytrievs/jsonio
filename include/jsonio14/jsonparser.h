#pragma once

#include "jsonbuilder.h"

namespace jsonio14 {


/// Class for read/write json structure from/to text file or string
class JsonParser
{

    const std::size_t err_block_size{20};

protected:

    /// Text to parse
    std::string jsontext;
    std::size_t cur_pos{0};
    std::size_t end_pos{0};

    bool skip_space();
    bool skip_space_comment();
    bool parse_string( std::string& str );
    void parse_value( const std::string& name, JsonBuilderBase& builder );
    /// Parse internal jsontext string to json structure (with first {)
    void parse_object( JsonObjectBuilder& builder );
    /// Parse internal jsontext string to json structure (with first [)
    void parse_array( JsonArrayBuilder& builder );

public:

    explicit JsonParser()  { }
    /// Parse json string to internal structure data
    void parseTo( const std::string& jsondata, JsonBase& out );

};

} // namespace jsonio14
