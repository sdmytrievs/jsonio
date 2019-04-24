#include "jsonbuilder.h"
#include "exceptions.h"

namespace jsonio14 {

void JsonBuilderBase::append_scalar(const std::string &key, const std::string &value)
{
    long ival = 0;
    double dval=0.;

    if( value == "~" )
        current_json.append_node( key, JsonBase::Null, "null" );
    else
        if( value == "null" )
            current_json.append_node( key, JsonBase::Null, "null" );
        else
            if( value == "true" )
                current_json.append_node( key, JsonBase::Bool, "true" );
            else
                if( value == "false" )
                    current_json.append_node( key, JsonBase::Bool, "false" );
                else
                    if( is<long>( ival, value ) )
                        current_json.append_node( key, JsonBase::Null, v2string(ival) );
                    else
                        if( is<double>( dval, value ))
                            current_json.append_node( key, JsonBase::Double, v2string(dval) );
                        else
                            current_json.append_node( key, JsonBase::String, v2string(value) );
}


JsonObjectBuilder JsonObjectBuilder::addObject(const std::string &akey)
{
    decltype(current_json) new_json = current_json.append_node( akey, JsonBase::Object, "" );
    return JsonObjectBuilder{ new_json };
}

JsonArrayBuilder JsonObjectBuilder::addArray(const std::string &akey)
{
    decltype(current_json) new_json = current_json.append_node( akey, JsonBase::Array, "" );
    return JsonArrayBuilder{ new_json };
}


JsonObjectBuilder JsonArrayBuilder::addObject()
{
    decltype(current_json) new_json = current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Object, "" );
    return JsonObjectBuilder{ new_json };
}

JsonArrayBuilder JsonArrayBuilder::addArray()
{
    decltype(current_json) new_json = current_json.append_node( std::to_string( current_json.getChildrenCount() ), JsonBase::Array, "" );
    return JsonArrayBuilder{ new_json };
}

JsonObjectBuilder JsonArrayBuilder::addObject(const std::string &akey)
{
    JARANGO_THROW_IF( akey != std::to_string( current_json.getChildrenCount() ),
                      "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addObject();
}

JsonArrayBuilder JsonArrayBuilder::addArray(const std::string &akey)
{
    JARANGO_THROW_IF( akey != std::to_string( current_json.getChildrenCount() ),
                      "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addArray();
}

JsonBuilderBase &JsonArrayBuilder::addString(const std::string &akey, const std::string &value)
{
    JARANGO_THROW_IF( akey != std::to_string( current_json.getChildrenCount() ),
                      "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addString(value);
}

JsonBuilderBase &JsonArrayBuilder::addScalar(const std::string &akey, const std::string &value)
{
    JARANGO_THROW_IF( akey != std::to_string( current_json.getChildrenCount() ),
                      "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addScalar(value);
}


} // namespace jsonio14
