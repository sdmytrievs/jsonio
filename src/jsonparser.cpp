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

void JsonParser::parseTo(const std::string &jsondata, JsonBase &out)
{
    jsontext = jsondata;
    cur_pos = 0;
    end_pos = jsondata.length();
    skip_space_comment();

    if( jsontext[cur_pos] == jsBeginObject)
    {
        JsonObjectBuilder jsBuilder(out);
        parse_object( jsBuilder );
    }
    else if( jsontext[cur_pos] == jsBeginArray)
    {
        JsonArrayBuilder jsBuilder(out);
        parse_array( jsBuilder );
    }
}

//  object = { "<key1>" : <value1>, ... , "<keyN>" : <valueN> }
void JsonParser::parse_object(JsonObjectBuilder &builder)
{
    std::string keyn;
    if( jsontext[cur_pos] == jsBeginObject )
        cur_pos++;
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 1, "unterminated object: " + jsontext.substr(cur_pos, err_block_size) );

    // empty object
    if( jsontext[cur_pos] == jsEndObject )
    {
        cur_pos++;
        return;
    }

    do {
        if( !parse_string( keyn ) )
            JARANGO_THROW(  "JsonParser", 2, "missing key of object: " + jsontext.substr(cur_pos, err_block_size) );

        JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 3, "missing value of object: " + jsontext.substr(cur_pos, err_block_size) );

        if( jsontext[cur_pos] == jsNameSeparator  )
            cur_pos++;
        else
            JARANGO_THROW(  "JsonParser", 4, "missing ':' : " + jsontext.substr(cur_pos, err_block_size) );

        parse_value( keyn,  builder );
        JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 1, "unterminated object: " + jsontext.substr(cur_pos, err_block_size) );

        if( jsontext[cur_pos] == jsEndObject  )
        {
            cur_pos++;
            return;
        }

    }while( jsontext[cur_pos++] == jsValueSeparator );

    JARANGO_THROW(  "JsonParser", 5, "illegal symbol : " + jsontext.substr(cur_pos, err_block_size) );
}

//    array = [ <value1>, ... <valueN> ]
void JsonParser::parse_array(JsonArrayBuilder &builder)
{
    if( jsontext[cur_pos] == jsBeginArray )
        cur_pos++;
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 10, "unterminated array: " + jsontext.substr(cur_pos, err_block_size) );

    // empty array
    if( jsontext[cur_pos] == jsEndArray )
    {
        cur_pos++;
        return;
    }

    do {
        parse_value( builder.nextKey(),  builder );
        JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 12, "unterminated array: " + jsontext.substr(cur_pos, err_block_size) );
        if( jsontext[cur_pos] == jsEndArray  )
        {
            cur_pos++;
            return;
        }
    }while( jsontext[cur_pos++] == jsValueSeparator );

    JARANGO_THROW(  "JsonParser", 13, "illegal symbol : " + jsontext.substr(cur_pos, err_block_size) );
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
    str = "";
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 21, "must be string: " + jsontext.substr(cur_pos, err_block_size) );

    if( jsontext[cur_pos++] != jsQuote )
        return false;

    while( jsontext[cur_pos] != jsQuote || jsontext[cur_pos-1] == '\\' )
    {
        str += jsontext[cur_pos];
        cur_pos++;
        if( cur_pos>=end_pos )
            return false;
    }
    cur_pos++;
    // conwert all pair ('\\''\n') to one simbol '\n' and other
    //convertReadedString( str );
    return true;
}

void JsonParser::parse_value(const std::string &name, JsonBuilderBase &builder)
{
    JARANGO_THROW_IF( !skip_space_comment(), "JsonParser", 41, "must be value: " + jsontext.substr(cur_pos, err_block_size) );

    switch( jsontext[cur_pos] )
    {
    case jsQuote:
    {
        std::string str;
        parse_string( str );
        // conwert all pair ('\\''\n') to one simbol '\n' and other
        //convertReadedString( str );
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

    case jsEndArray:
        --cur_pos;
        break;  // empty array
    case jsEndObject:
        --cur_pos;
        break;  // empty object

    default:  // addScalar true/false/null/number
    {
        auto pos_end_value = jsontext.find_first_of( ",]}", cur_pos );
        auto valuestr = jsontext.substr(cur_pos, pos_end_value);
        trim(valuestr);
        builder.testScalar( name, valuestr );
        cur_pos = pos_end_value;
    }
        break;
    }
}


} // namespace jsonio14
