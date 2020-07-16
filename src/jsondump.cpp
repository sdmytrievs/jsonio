#include <iomanip>
#include "jsonio17/jsondump.h"
#include "jsonio17/jsonfree.h"
#include "jsonio17/jsonschema.h"
#include "jsonio17/service.h"


namespace jsonio17 {

namespace json {

/* encode_utf8(pt, out)
 *
 * Encode pt as UTF-8 and add it to out.
 */
void encode_utf8(long pt, std::string & out);

std::string dump(const JsonBase &object, bool dense)
{
    return object.dump(dense);
}

void dump(std::ostream &os, const JsonBase &object, bool dense)
{
    object.dump( os, dense );
}

void loads(const std::string &jsonstr, JsonBase &object)
{
    object.loads(jsonstr);
}

JsonFree loads(const std::string &jsonstr)
{
    auto object = JsonFree::object();
    object.loads(jsonstr);
    return object;
}


JsonSchema loads(const std::string& schema_name,const std::string &jsonstr)
{
    auto object = JsonSchema::object(schema_name);
    object.loads(jsonstr);
    return object;
}

std::string dump(const std::string &value )
{
    std::string out;
    out += '"';
    for (size_t i = 0; i < value.length(); i++) {
        const char ch = value[i];
        if (ch == '\\') {
            out += "\\\\";
        } else if (ch == '"') {
            out += "\\\"";
        } else if (ch == '\b') {
            out += "\\b";
        } else if (ch == '\f') {
            out += "\\f";
        } else if (ch == '\n') {
            out += "\\n";
        } else if (ch == '\r') {
            out += "\\r";
        } else if (ch == '\t') {
            out += "\\t";
        } else if (static_cast<uint8_t>(ch) <= 0x1f) {
            char buf[8];
            snprintf(buf, sizeof buf, "\\u%04x", ch);
            out += buf;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa8) {
            out += "\\u2028";
            i += 2;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa9) {
            out += "\\u2029";
            i += 2;
        } else {
            out += ch;
        }
    }
    out += '"';

    return out;
}

std::string dump(const char *value )
{
    return dump( std::string(value) );
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


///  @brief scan a string literal
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
                    JSONIO_THROW_IF( esc.length() < 4, "JsonParser", 10, "bad \\u escape: " + esc );
                    for (size_t j = 0; j < 4; j++) {
                        JSONIO_THROW_IF( !in_range(esc[j], 'a', 'f') and !in_range(esc[j], 'A', 'F') and !in_range(esc[j], '0', '9'),
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
                    JSONIO_THROW( "JsonParser", 13, "invalid escape character " + esc(ch) );
                }
            }
            else
            {
                encode_utf8(last_escaped_codepoint, resstr);
                last_escaped_codepoint = -1;
                auto ch = strvalue[ii++];
                JSONIO_THROW_IF( in_range<long>(ch, 0, 0x1f),
                                  "JsonParser", 12, "unescaped in string" );
                resstr += ch;
            }
        }
        encode_utf8(last_escaped_codepoint, resstr);
        strvalue =  resstr;
    }
}


} // json namespace

} // namespace jsonio17

