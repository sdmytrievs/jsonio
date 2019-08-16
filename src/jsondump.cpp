#include <iomanip>
#include "jsondump.h"
#include "jsonparser.h"
#include "jsonfree.h"
#include "service.h"


namespace jsonio14 {

namespace json {

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

void dump2stream( std::ostream& os, const JsonBase& object, int depth, bool dense )
{
    int temp;
    bool first = true;
    auto objtype = object.getType();
    auto objsize = object.getChildrenCount();

    for( std::size_t ii=0; ii<objsize; ii++ )
    {
        auto childobj = object.getChild( ii);

        // do not print empty data
        if( childobj->getKey() == "_key" && childobj->getFieldValue().empty() )
            continue;
        if( childobj->getKey() == "_id" && childobj->getFieldValue().empty() )
            continue;

        if( !first )
            os << ( dense ? "," : ",\n" );
        else
            first = false;

        // before print
        switch( objtype )
        {
        case JsonBase::Object:
            if(!dense)
            {
                for (temp = 0; temp <= depth; temp++)
                    os <<  "     ";
            }
            os << "\"" << childobj->getKey() << ( dense ? "\":" : "\" :   " );
            break;
        case JsonBase::Array:
            if(!dense)
            {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            break;
        default:
            break;
        }

        switch (childobj->getType())
        {
        // impotant datatypes
        case JsonBase::Null:
            os << "null";
            break;
        case JsonBase::Bool:
        case JsonBase::Int:
            os << childobj->getFieldValue();
            break;
        case JsonBase::Double:
            os << std::setprecision(DetailSettings::doublePrecision) << childobj->toDouble();
            break;
        case JsonBase::String:
            os << dump( childobj->getFieldValue() );
            break;

            // main constructions
        case JsonBase::Object:
            os << ( dense ? "{" : "{\n" );
            dump2stream( os, *childobj, depth + 1, dense );
            if(!dense)
            {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            os << "}";
            break;
        case JsonBase::Array:
            os << ( dense ? "[" : "[\n" );
            dump2stream(os, *childobj, depth + 1, dense );
            if(!dense)
            {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            os << "]";
            break;
        default:
            os  << "can't print type : " << childobj->getType();
        }
    }
    if( !dense )
        os << "\n";
}


std::string dump( const JsonBase &object, bool dense )
{
    std::stringstream os;
    dump(os, object, dense);
    return os.str();
}

void dump( std::ostream &os, const JsonBase &object, bool dense )
{
    auto objtype = object.getType();
    if( objtype == JsonBase::Object )
        os << ( dense ? "{" : "{\n" );
    else
        if( objtype == JsonBase::Array )
            os << ( dense ? "[" : "[\n" );
        else
            if( objtype == JsonBase::String )
            {
                os << dump( object.getFieldValue() );
                return;
            }
            else
            {
                os << object.getFieldValue();
                return;
            }

    dump2stream( os, object, 0, dense );
    if( objtype == JsonBase::Object )
        os << "}\n";
    else
        os << "]\n";
}

void loads(const std::string &jsonstr, JsonBase &object)
{
    JsonParser parser(jsonstr);
    parser.parse_to(object);
}

JsonFree loads(const std::string &jsonstr)
{
    auto object = JsonFree::object();
    JsonParser parser(jsonstr);
    parser.parse_to(object);
    return object;
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

} // json namespace

} // namespace jsonio14

