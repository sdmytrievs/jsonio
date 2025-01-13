#include <iomanip>
#include "jsonio/jsondump.h"
#include "jsonio/jsonparser.h"

namespace jsonio {


std::ostream &operator<<( std::ostream &os, const JsonBase &obj )
{
    obj.dump( os );
    return os;
}

template <> JsonBase::Type JsonBase::typeTraits( const bool& )
{
    return JsonBase::Bool;
}

template <> JsonBase::Type JsonBase::typeTraits( const char& )
{
    return JsonBase::String;
}

template <> JsonBase::Type JsonBase::typeTraits( const std::string& )
{
    return JsonBase::String;
}


std::string JsonBase::toString( bool dense ) const
{
    if( isStructured() )
    {
        return dump( dense );
    }
    return getFieldValue();
}

double JsonBase::toDouble() const
{
    double val = 0.0;
    if( isNumber() )
    {
        string2v( getFieldValue(), val );
    }
    return val;
}

long JsonBase::toInt() const
{
    long val = 0;
    if( type() == Type::Int )
    {
        //double dval = 0.0;
        string2v( getFieldValue(), val );
        //val = static_cast<long>(dval);
    }
    return val;
}

bool JsonBase::toBool() const
{
    bool val = false;
    if( isBool() )
    {
        string2v( getFieldValue(), val );
    }
    return val;
}

std::string JsonBase::getHelpName() const
{
    auto item = this;
    while( item->getParent() != nullptr && item->getParent()->isArray() )
    {
        item = item->getParent();
    }
    return item->getKey();
}

/// Get Field Path from Node
std::string JsonBase::get_path() const
{
    if( isTop() || getParent()->isTop()  )
    {
        return getKey();
    }
    else
    {
        return  getParent()->get_path()+"."+ getKey();
    }
}

bool JsonBase::set_scalar_via_path(const std::string &jsonpath, const std::string &value)
{
    long ival = 0;
    double dval=0.;

    auto pobj = field_add(jsonpath);
    if( pobj ) {

        if( value == "~" )
            pobj->update_node(JsonBase::Null, "null");
        else
            if( value == "null" )
                pobj->update_node(JsonBase::Null, "null");
            else
                if( value == "true" )
                    pobj->update_node(JsonBase::Bool, "true");
                else
                    if( value == "false" )
                        pobj->update_node(JsonBase::Bool, "false");
                    else
                        if( is<long>( ival, value ) )
                            pobj->update_node(JsonBase::Int, v2string(ival));
                        else
                            if( is<double>( dval, value ))
                                pobj->update_node(JsonBase::Double, v2string(dval));
                            else
                                pobj->update_node(JsonBase::String, v2string(value));
        return true;
    }
    return false;
}

void JsonBase::set_oid(const std::string &oid )
{
    JSONIO_THROW_IF( !isTop(), "JsonBase", 14, "cannot add _id on level." );

    if( !oid.empty() )
    {
        auto  parts = split( oid, "/");
        set_value_via_path( "_key", parts.back() );
        set_value_via_path( "_id", oid );
    }
    else
    {
        // only clear
        set_value_via_path( "_key", oid );
        set_value_via_path( "_id", oid );
    }
}

const char *JsonBase::typeName(JsonBase::Type type)
{
    switch( type )
    {
    case Int:
        return "int";
    case Double:
        return "double";
    case Null:
        return "null";
    case Object:
        return "object";
    case Array:
        return "array";
    case String:
        return "string";
    case Bool:
        return "bool";
    }
    return "";
}


void JsonBase::dump2stream( std::ostream& os, int depth, bool dense ) const
{
    int temp;
    bool first = true;
    auto objtype = type();
    auto objsize = getChildrenCount();

    for( std::size_t ii=0; ii<objsize; ii++ )
    {
        auto childobj = getChild( ii);

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

        switch (childobj->type())
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
            os << json::dump( childobj->getFieldValue() );
            break;

            // main constructions
        case JsonBase::Object:
            os << ( dense ? "{" : "{\n" );
            childobj->dump2stream( os, depth + 1, dense );
            if(!dense)
            {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            os << "}";
            break;
        case JsonBase::Array:
            os << ( dense ? "[" : "[\n" );
            childobj->dump2stream(os, depth + 1, dense );
            if(!dense)
            {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            os << "]";
            break;
        default:
            os  << "can't print type : " << childobj->type();
        }
    }
    if( !dense )
        os << "\n";
}

JsonBase *JsonBase::field_add(const std::string &fieldpath )
{
    auto names = split(fieldpath, field_path_delimiters);
    return field_add_names(names);
}

std::string JsonBase::dump(bool dense) const
{
    std::stringstream os;
    dump(os,  dense);
    return os.str();
}

void JsonBase::dump( std::ostream &os, bool dense ) const
{
    auto objtype = type();
    if( objtype == JsonBase::Object )
        os << ( dense ? "{" : "{\n" );
    else
        if( objtype == JsonBase::Array )
            os << ( dense ? "[" : "[\n" );
        else
            if( objtype == JsonBase::String )
            {
                os << json::dump( getFieldValue() );
                return;
            }
            else
            {
                os << getFieldValue();
                return;
            }

    dump2stream( os, 0, dense );
    if( objtype == JsonBase::Object )
        os << "}";
    else
        os << "]";
}

void JsonBase::loads( const std::string &jsonstr )
{
    JsonParser parser(jsonstr);
    parser.parse_to(this);
}

std::string JsonBase::checked_value( JsonBase::Type type, const std::string& newvalue )
{
    std::string chvalue = newvalue;
    switch ( type )
    {
    case Null:
        chvalue = "null";
        break;
    case Bool:
        if( chvalue != "true" )
            chvalue = "false";
        break;
    case Int: {
        int ival;
        if( !string2v( chvalue, ival ) )
            chvalue = "0";
    }
        break;
    case Double: {
        double dval;
        if( !string2v( chvalue, dval ) )
            chvalue = "0.0";
    }
        break;
    case String:
        if( chvalue.empty() )
            chvalue = "";//emptiness;
        break;
        // main constructions
    case Object:
    case Array:
    default:
        chvalue = "";
        break;
    }
    return chvalue;
}

} // namespace jsonio
