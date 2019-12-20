#pragma once

#include "jsonbuilder.h"

namespace jsonio14 {


/// Class for read JsonBase structure from json string.
class JsonParser
{

    const std::size_t err_block_size{20};

public:

    /// Constructor
    explicit JsonParser( const std::string &jsondata ):jsontext()
    {
     set_string( jsondata );
    }

    /// Update string to parse.
    void set_string( const std::string &jsondata )
    {
       jsontext = jsondata;
       cur_pos = 0;
       end_pos = jsondata.length();
    }

    /// Parse internal string to the JSON object result_object;
    void parse_to( JsonBase& result_object );

protected:

    /// Internal string to parse
    std::string jsontext;
    std::size_t cur_pos{0};
    std::size_t end_pos{0};

    bool skip_space();
    bool skip_space_comment();
    bool parse_string( std::string& str );
    void parse_value( int depth, const std::string& name, JsonBuilderBase& builder );
    /// Parse internal jsontext string to json structure (with first {)
    void parse_object( int depth, JsonObjectBuilder& builder );
    /// Parse internal jsontext string to json structure (with first [)
    void parse_array( int depth, JsonArrayBuilder& builder );

    /// Get substr with error
    std::string err_part() const;
    void set_scalar(JsonBase &result_object );

};

} // namespace jsonio14
