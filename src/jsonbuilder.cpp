#include "jsonio17/jsonbuilder.h"

namespace jsonio17 {


// decision about value type
void JsonBuilderBase::append_scalar(const std::string &key, const std::string &value, bool noString )
{
    long ival = 0;
    double dval=0.;

    if( !current_json )
        return;

    if( value == "~" )
        current_json->append_node( key, JsonBase::Null, "null" );
    else
        if( value == "null" )
            current_json->append_node( key, JsonBase::Null, "null" );
        else
            if( value == "true" )
                current_json->append_node( key, JsonBase::Bool, "true" );
            else
                if( value == "false" )
                    current_json->append_node( key, JsonBase::Bool, "false" );
                else
                    if( is<long>( ival, value ) )
                        current_json->append_node( key, JsonBase::Int, v2string(ival) );
                    else
                        if( is<double>( dval, value ))
                            current_json->append_node( key, JsonBase::Double, v2string(dval) );
                        else
                            if( noString )
                                JSONIO_THROW(  "JsonArrayBuilder", 4, key + " undefined value type '" + value +"'" );
                            else
                                current_json->append_node( key, JsonBase::String, v2string(value) );
}

JsonBuilderBase &JsonBuilderBase::testScalar(const std::string &key, const std::string &value)
{
    append_scalar( key,  value, true  );
    return *this;
}


JsonObjectBuilder JsonObjectBuilder::addObject(const std::string &akey)
{
    //decltype(current_json) new_json = current_json.append_node( akey, JsonBase::Object, "" );
    decltype(current_json) new_json = nullptr;
    if( current_json )
        new_json = current_json->append_node( akey, JsonBase::Object, "" );

    return JsonObjectBuilder{ new_json };
}

JsonArrayBuilder JsonObjectBuilder::addArray(const std::string &akey)
{
    //decltype(current_json) new_json = current_json.append_node( akey, JsonBase::Array, "" );
    decltype(current_json) new_json = nullptr;
    if( current_json )
        new_json = current_json->append_node( akey, JsonBase::Array, "" );
    return JsonArrayBuilder{ new_json };
}


JsonObjectBuilder JsonArrayBuilder::addObject()
{
//    decltype(current_json) new_json = current_json.append_node( nextKey(), JsonBase::Object, "" );
    decltype(current_json) new_json = nullptr;
    if( current_json )
        new_json = current_json->append_node( nextKey(), JsonBase::Object, "" );
    return JsonObjectBuilder{ new_json };
}

JsonArrayBuilder JsonArrayBuilder::addArray()
{
//    decltype(current_json) new_json = current_json.append_node( nextKey(), JsonBase::Array, "" );
    decltype(current_json) new_json = nullptr;
    if( current_json )
        new_json = current_json->append_node( nextKey(), JsonBase::Array, "" );
    return JsonArrayBuilder{ new_json };
}

JsonObjectBuilder JsonArrayBuilder::addObject(const std::string &akey)
{
    JSONIO_THROW_IF( akey != nextKey(), "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addObject();
}

JsonArrayBuilder JsonArrayBuilder::addArray(const std::string &akey)
{
    JSONIO_THROW_IF( akey != nextKey(), "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addArray();
}

JsonBuilderBase &JsonArrayBuilder::addString(const std::string &akey, const std::string &value)
{
    JSONIO_THROW_IF( akey != nextKey(), "JsonArrayBuilder", 1, "cannot use array with key " + akey );
    return addString(value);
}


} // namespace jsonio17
