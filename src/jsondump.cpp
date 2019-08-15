#include <iomanip>
#include "jsondump.h"
#include "jsonparser.h"
#include "jsonfree.h"

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

} // json namespace

} // namespace jsonio14

