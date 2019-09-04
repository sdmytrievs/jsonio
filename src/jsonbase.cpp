#include <iomanip>
#include "jsondump.h"
#include "jsonparser.h"

namespace jsonio14 {


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

const char *JsonBase::typeName(JsonBase::Type type)
{
    switch( type )
    {
    case Null:
        return "null";
    case Object:
        return "object";
    case Array:
        return "array";
    case String:
        return "string";
    case Bool:
        return "boolean";
    case UNDEFINED:
        return "undefined";
    default:
        return "number";
    }
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

JsonBase *JsonBase::field(const std::string &fieldpath) const
{
    auto names = split(fieldpath, "./[]\"");
    return field(names);
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
        os << "}\n";
    else
        os << "]\n";
}

void JsonBase::loads( const std::string &jsonstr )
{
    JsonParser parser(jsonstr);
    parser.parse_to(*this);
}


/* Temporaly not used
 *
 *

/// JsonBase creation factory
template <class TJson>
class JsonFactory
{
    static const  std::string default_key;
public:

    JsonFactory() {}

    static std::shared_ptr<TJson> create()
    {
        return std::make_shared<TJson>(JsonBase::Null, default_key, "");
    }

    static std::shared_ptr<TJson> create( bool value )
    {
        return std::make_shared<TJson>(JsonBase::Bool, default_key, dumps(value));
    }

    static std::shared_ptr<TJson> create( long value )
    {
        return std::make_shared<TJson>(JsonBase::Int, default_key, dumps(value));
    }

    static std::shared_ptr<TJson> create( double value )
    {
        return std::make_shared<TJson>(JsonBase::Double, default_key, dumps(value));
    }

    static std::shared_ptr<TJson> create( const std::string& value )
    {
        return std::make_shared<TJson>(JsonBase::String, default_key, dumps(value));
    }

    // Implicit constructor: map-like objects (std::map, std::unordered_map, etc)
    template <class Map, typename std::enable_if<
                  std::is_constructible<std::string, decltype(std::declval<Map>().begin()->first)>::value
                  && std::is_constructible<TJson, decltype(std::declval<Map>().begin()->second)>::value, int>::type = 0>
    static std::shared_ptr<TJson> create( const Map& values )
    {
        auto obj = std::make_shared<TJson>(JsonBase::Object, default_key, "");
        for( const auto& el: values )
        {
            auto newChild = std::make_shared<TJson>(el.second);
            newChild->field_key = el.first;
            newChild->ndx_in_parent = obj.children.size();
            obj.children.push_back( std::move(newChild) );
        }
        return obj;
    }

    // Implicit constructor: vector-like objects (std::list, std::vector, std::set, etc)
    template <class Container, typename std::enable_if<
                  std::is_constructible<TJson, decltype(*std::declval<Container>().begin())>::value, int>::type = 0>
    static std::shared_ptr<TJson> create( const Container& values )
    {
        auto arr = std::make_shared<TJson>(JsonBase::Array, default_key, "");
        int ii=0;
        for( const auto& el: values )
        {
            auto newChild = std::make_shared<TJson>(el);
            newChild->field_key = std::to_string(ii++);
            newChild->ndx_in_parent = arr.children.size();
            arr.children.push_back( std::move(newChild) );
        }
        return arr;
    }

};

*/

} // namespace jsonio14
