#include "jsonparser.h"
#include "exceptions.h"
#include "service.h"


namespace jsonio14 {

enum {
    jsBeginArray = '[',    //0x5b,
    jsBeginObject = '{',   //0x7b,
    jsEndArray = ']',      //0x5d,
    jsEndObject = '}',     //0x7d,
    jsNameSeparator = ':', //0x3a,
    jsValueSeparator = ',',//0x2c,
    jsQuote = '\"'      //0x22
};

void undumpString( std::string& strvalue );

void JsonParser::parse_to( JsonBase &out )
{
    cur_pos = 0;
    skip_space_comment();

    if( jsontext[cur_pos] == jsBeginObject )
    {
        JsonObjectBuilder jsBuilder(out);
        parse_object( jsBuilder );
    }
    else if( jsontext[cur_pos] == jsBeginArray )
    {
        JsonArrayBuilder jsBuilder(out);
        parse_array( jsBuilder );
    }
    // else ??? update value Scalar
    skip_space_comment();
    JARANGO_THROW_IF( cur_pos < end_pos, "JsonParser", 14, "extra value after close: " + err_part() );
}

//  object = { "<key1>" : <value1>, ... , "<keyN>" : <valueN> }
void JsonParser::parse_object(JsonObjectBuilder &builder)
{
    std::string keyn;
    if( jsontext[cur_pos] == jsBeginObject )
        cur_pos++;
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 1, "unterminated object: " + err_part() );

    // empty object
    if( jsontext[cur_pos] == jsEndObject )
    {
        cur_pos++;
        return;
    }

    do {
        if( !parse_string( keyn ) )
            JARANGO_THROW(  "JsonParser", 2, "missing key of object: " + err_part() );

        JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 3, "missing value of object: " + err_part() );

        if( jsontext[cur_pos] == jsNameSeparator  )
            cur_pos++;
        else
            JARANGO_THROW(  "JsonParser", 4, "missing ':' : " + err_part() );

        parse_value( keyn,  builder );
        JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 1, "unterminated object: " + err_part() );

        if( jsontext[cur_pos] == jsEndObject  )
        {
            cur_pos++;
            return;
        }

    }while( jsontext[cur_pos++] == jsValueSeparator );

    JARANGO_THROW(  "JsonParser", 5, "illegal symbol : '" + jsontext.substr(cur_pos-1, err_block_size)+"'" );
}

//    array = [ <value1>, ... <valueN> ]
void JsonParser::parse_array(JsonArrayBuilder &builder)
{
    if( jsontext[cur_pos] == jsBeginArray )
        cur_pos++;
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 6, "unterminated array: " + err_part() );

    // empty array
    if( jsontext[cur_pos] == jsEndArray )
    {
        cur_pos++;
        return;
    }

    do {
        parse_value( builder.nextKey(),  builder );
        JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 6, "unterminated array: " + err_part() );
        if( jsontext[cur_pos] == jsEndArray  )
        {
            cur_pos++;
            return;
        }
    }while( jsontext[cur_pos++] == jsValueSeparator );

    JARANGO_THROW(  "JsonParser", 5, "illegal symbol : '" + jsontext.substr(cur_pos-1, err_block_size)+"'" );
}

std::string JsonParser::err_part() const
{
    std::string asubstr;
    if( cur_pos < end_pos - err_block_size)
        asubstr =  jsontext.substr(cur_pos, err_block_size);
    else if( end_pos < err_block_size )
        asubstr = jsontext;
    else
        asubstr = jsontext.substr( end_pos - err_block_size);

    return "'" + asubstr  + "'";
}


bool JsonParser::skip_space()
{
    for( ; cur_pos<end_pos; ++cur_pos )
    {
        if( !std::isspace(jsontext[cur_pos]) )
            break;
    }
    return (cur_pos < end_pos);
}

bool JsonParser::skip_space_comment()
{
    bool haznext = skip_space();
    while( haznext && jsontext[cur_pos] == '#' )
    {
        cur_pos = jsontext.find_first_of( "\n", cur_pos );
        if( cur_pos == std::string::npos )
            return  false;
        haznext = skip_space();
    }
    return haznext;
}

bool JsonParser::parse_string( std::string &str )
{
    bool lastCh = false;
    str = "";
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 7, "must be string: " + err_part() );

    if( jsontext[cur_pos++] != jsQuote )
        return false;

    while( jsontext[cur_pos] != jsQuote || lastCh )
    {
        str += jsontext[cur_pos];
        if( jsontext[cur_pos] == '\\' )
            lastCh = !lastCh;
        else
            lastCh = false;

        cur_pos++;
        if( cur_pos>=end_pos )
            return false;
    }
    cur_pos++;
    // conwert all pair ('\\''\n') to one simbol '\n' and other
    //undumpString( str );
    return true;
}

void JsonParser::parse_value(const std::string &name, JsonBuilderBase &builder)
{
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 8, "must be value " + err_part() );

    switch( jsontext[cur_pos] )
    {
    case jsQuote:
    {
        std::string str;
        parse_string( str );
        // conwert all pair ('\\''\n') to one simbol '\n' and other
        undumpString( str );
        builder.addString( name, str );
    }
        break;

    case jsBeginArray:
    {
        auto childobj =  builder.addArray(  name );
        parse_array( childobj );
    }
        break;

    case jsBeginObject:
    {
        auto childobj =  builder.addObject( name );
        parse_object( childobj );
    }
        break;

    default:  // addScalar true/false/null/number
    {
        auto pos_end_value = jsontext.find_first_of( ",]}", cur_pos );
        auto end_size = pos_end_value;
        if( pos_end_value != std::string::npos )
        {
           end_size -= cur_pos;
        }
        auto valuestr = jsontext.substr(cur_pos, end_size);
        trim(valuestr);
        JARANGO_THROW_IF( valuestr.empty(), "JsonParser", 8, "must be value " + err_part() );
        builder.testScalar( name, valuestr );
        cur_pos = pos_end_value;
    }
        break;
    }
}

// use decoder --------------------------------------------
// https://github.com/dropbox/json11/blob/master/json11.cpp
// see other https://github.com/nlohmann/json/blob/develop/include/nlohmann/detail/input/lexer.hpp

/* esc(c)
 *
 * Format char c suitable for printing in an error message.
 */
static inline std::string esc(char c) {
    char buf[12];
    if (static_cast<uint8_t>(c) >= 0x20 && static_cast<uint8_t>(c) <= 0x7f) {
        snprintf(buf, sizeof buf, "'%c' (%d)", c, c);
    } else {
        snprintf(buf, sizeof buf, "(%d)", c);
    }
    return std::string(buf);
}

/* encode_utf8(pt, out)
 *
 * Encode pt as UTF-8 and add it to out.
 */
void encode_utf8(long pt, std::string & out) {
    if (pt < 0)
        return;

    if (pt < 0x80) {
        out += static_cast<char>(pt);
    } else if (pt < 0x800) {
        out += static_cast<char>((pt >> 6) | 0xC0);
        out += static_cast<char>((pt & 0x3F) | 0x80);
    } else if (pt < 0x10000) {
        out += static_cast<char>((pt >> 12) | 0xE0);
        out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
        out += static_cast<char>((pt & 0x3F) | 0x80);
    } else {
        out += static_cast<char>((pt >> 18) | 0xF0);
        out += static_cast<char>(((pt >> 12) & 0x3F) | 0x80);
        out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
        out += static_cast<char>((pt & 0x3F) | 0x80);
    }
}

void undumpString( std::string& strvalue )
{
    if(strvalue.empty())
        return;

    if( strvalue.find_first_of("\\") != std::string::npos )
    {
        std::string resstr = "";
        size_t ii=0;
        long last_escaped_codepoint = -1;

        while( ii<strvalue.length()  )
        {
            if( strvalue[ii]  == '\\')
            {
                ii++;
                auto ch = strvalue[ii++];

                if( ch == 'u')
                {
                    // Extract 4-byte escape sequence
                    std::string esc = strvalue.substr(ii, 4);
                    // Explicitly check length of the substring. The following loop
                    // relies on std::string returning the terminating NUL when
                    // accessing str[length]. Checking here reduces brittleness.
                    JARANGO_THROW_IF( esc.length() < 4, "JsonParser", 10, "bad \\u escape: " + esc );
                    for (size_t j = 0; j < 4; j++) {
                        JARANGO_THROW_IF( !in_range(esc[j], 'a', 'f') and !in_range(esc[j], 'A', 'F') and !in_range(esc[j], '0', '9'),
                                          "JsonParser", 11, "bad \\u escape: " + esc );
                    }
                    long codepoint = strtol(esc.data(), nullptr, 16);

                    // JSON specifies that characters outside the BMP shall be encoded as a pair
                    // of 4-hex-digit \u escapes encoding their surrogate pair components. Check
                    // whether we're in the middle of such a beast: the previous codepoint was an
                    // escaped lead (high) surrogate, and this is a trail (low) surrogate.
                    if (in_range<long>(last_escaped_codepoint, 0xD800, 0xDBFF)
                            && in_range<long>(codepoint, 0xDC00, 0xDFFF)) {
                        // Reassemble the two surrogate pairs into one astral-plane character, per
                        // the UTF-16 algorithm.
                        encode_utf8((((last_escaped_codepoint - 0xD800) << 10)
                                     | (codepoint - 0xDC00)) + 0x10000, resstr);
                        last_escaped_codepoint = -1;
                    } else {
                        encode_utf8(last_escaped_codepoint, resstr);
                        last_escaped_codepoint = codepoint;
                    }

                    ii += 4;
                    continue;
                }

                encode_utf8(last_escaped_codepoint, resstr);
                last_escaped_codepoint = -1;

                switch( ch )
                {
                case '\"': resstr += '\"'; break;
                case '\'': resstr += '\''; break;
                case '\\': resstr += '\\'; break;
                case '/':  resstr += '/';  break;
                case 'n':  resstr += '\n'; break;
                case 'r':  resstr += '\r'; break;
                case 't':  resstr += '\t'; break;
                case 'b':  resstr += '\b'; break;
                case 'f':  resstr += '\f'; break;
                default:
                    JARANGO_THROW( "JsonParser", 13, "invalid escape character " + esc(ch) );
                }
            }
            else
            {
                encode_utf8(last_escaped_codepoint, resstr);
                last_escaped_codepoint = -1;
                auto ch = strvalue[ii++];
                JARANGO_THROW_IF( in_range<long>(ch, 0, 0x1f),
                                  "JsonParser", 12, "unescaped in string" );
                resstr += ch;
            }
        }
        encode_utf8(last_escaped_codepoint, resstr);
        strvalue =  resstr;
    }
}


} // namespace jsonio14
