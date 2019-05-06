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

    /// Get substr with error
    std::string err_part() const;

public:

    explicit JsonParser(const std::string &jsondata):
        jsontext{jsondata}, cur_pos{0}, end_pos{jsondata.length()}
    { }
    /// Parse json string to internal structure data
    void parse_to( JsonBase& out );

};

} // namespace jsonio14
