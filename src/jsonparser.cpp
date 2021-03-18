#include <locale>
#include "jsonio17/jsonparser.h"
#include "jsonio17/jsondump.h"
#include "jsonio17/exceptions.h"
#include "jsonio17/service.h"
#include "jsonio17/jsonbuilder.h"

namespace jsonio17 {

static const int json_max_depth = 10000;

enum {
    jsBeginArray = '[',    //0x5b,
    jsBeginObject = '{',   //0x7b,
    jsEndArray = ']',      //0x5d,
    jsEndObject = '}',     //0x7d,
    jsNameSeparator = ':', //0x3a,
    jsValueSeparator = ',',//0x2c,
    jsQuote = '\"'      //0x22
};

void JsonParser::parse_to( JsonBase* out )
{
    cur_pos = 0;
    skip_space_comment();

    if( jsontext[cur_pos] == jsBeginObject )
    {
        JsonObjectBuilder jsBuilder(out);
        parse_object( 0, jsBuilder );
    }
    else if( jsontext[cur_pos] == jsBeginArray )
    {
        JsonArrayBuilder jsBuilder(out);
        parse_array( 0, jsBuilder );
    }
    else // update value Scalar
    {
      set_scalar( *out );
    }
    skip_space_comment();
    JSONIO_THROW_IF( cur_pos < end_pos, "JsonParser", 14, "extra value after close: " + err_part() );
}

//  object = { "<key1>" : <value1>, ... , "<keyN>" : <valueN> }
void JsonParser::parse_object( int depth, JsonObjectBuilder &builder)
{
    std::string keyn;
    if( jsontext[cur_pos] == jsBeginObject )
        cur_pos++;
    JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 1, "unterminated object: " + err_part() );

    // empty object
    if( jsontext[cur_pos] == jsEndObject )
    {
        cur_pos++;
        return;
    }

    do {
        if( !parse_string( keyn ) )
            JSONIO_THROW(  "JsonParser", 2, "missing key of object: " + err_part() );

        JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 3, "missing value of object: " + err_part() );

        if( jsontext[cur_pos] == jsNameSeparator  )
            cur_pos++;
        else
            JSONIO_THROW(  "JsonParser", 4, "missing ':' : " + err_part() );

        parse_value( depth, keyn,  builder );
        JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 1, "unterminated object: " + err_part() );

        if( jsontext[cur_pos] == jsEndObject  )
        {
            cur_pos++;
            return;
        }

    }while( jsontext[cur_pos++] == jsValueSeparator );

    JSONIO_THROW(  "JsonParser", 5, "illegal symbol : '" + jsontext.substr(cur_pos-1, err_block_size)+"'" );
}

//    array = [ <value1>, ... <valueN> ]
void JsonParser::parse_array( int depth, JsonArrayBuilder &builder)
{
    if( jsontext[cur_pos] == jsBeginArray )
        cur_pos++;
    JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 6, "unterminated array: " + err_part() );

    // empty array
    if( jsontext[cur_pos] == jsEndArray )
    {
        cur_pos++;
        return;
    }

    do {
        parse_value( depth, builder.nextKey(),  builder );
        JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 6, "unterminated array: " + err_part() );
        if( jsontext[cur_pos] == jsEndArray  )
        {
            cur_pos++;
            return;
        }
    }while( jsontext[cur_pos++] == jsValueSeparator );

    JSONIO_THROW(  "JsonParser", 5, "illegal symbol : '" + jsontext.substr(cur_pos-1, err_block_size)+"'" );
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
    JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 7, "must be string: " + err_part() );

    if( jsontext[cur_pos++] != jsQuote )
        return false;

    while( jsontext[cur_pos] != jsQuote || lastCh )
    {
        // non-UTF-8 sequence
        if (  !std::isspace(jsontext[cur_pos], std::locale()) && static_cast<uint8_t>(jsontext[cur_pos]) < 0x20)
        {
          // control character
          JSONIO_THROW( "JsonParser", 10, "Unexpected control character" + err_part() );
        }

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

void JsonParser::parse_value( int depth, const std::string &name, JsonBuilderBase &builder)
{
    JSONIO_THROW_IF( depth > json_max_depth, "JsonParser", 10, "exceeded maximum nesting depth " + err_part() );
    JSONIO_THROW_IF( !skip_space_comment(), "JsonParser", 8, "must be value " + err_part() );

    switch( jsontext[cur_pos] )
    {
    case jsQuote:
    {
        std::string str;
        parse_string( str );
        // conwert all pair ('\\''\n') to one simbol '\n' and other
        json::undumpString( str );
        builder.addString( name, str );
    }
        break;

    case jsBeginArray:
    {
        auto childobj =  builder.addArray(  name );
        parse_array( depth+1, childobj );
    }
        break;

    case jsBeginObject:
    {
        auto childobj =  builder.addObject( name );
        parse_object( depth+1, childobj );
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
        JSONIO_THROW_IF( valuestr.empty(), "JsonParser", 8, "must be value " + err_part() );
        builder.testScalar( name, valuestr, true );
        cur_pos = pos_end_value;
    }
        break;
    }
}

// decision about value type
void JsonParser::set_scalar( JsonBase& result_object )
{
    long ival = 0;
    double dval=0.;

    if( jsontext[cur_pos] == jsQuote )
    {
        std::string str;
        parse_string( str );
        json::undumpString( str );
        result_object.set_from( str );
        return;
    }

    std::string value(jsontext.substr(cur_pos));
    cur_pos = std::string::npos;

    if( value == "~" || value == "null" )
            result_object.set_null();
        else
            if( value == "true" )
                result_object.set_from( true );
            else
                if( value == "false" )
                    result_object.set_from( false );
                else
                    if( is<long>( ival, value ) )
                        result_object.set_from( ival );
                    else
                        if( is<double>( dval, value ))
                            result_object.set_from(  dval );
                        else
                            JSONIO_THROW(  "JsonParser", 9, "must be value " + value );
}

} // namespace jsonio17
