#include <regex>
#include "jsonio17/service.h"


namespace jsonio17 {

// How to split a string in C++.
std::vector<std::string> split2(const std::string& s, char delimiter);

// Returns whether the string matches the regular expression.
bool regexp_test(const std::string& str, std::string rgx_str)
{
    std::regex rx(rgx_str);
    return std::regex_match( str , rx );
}

//  Function that can be used to split text using regexp.
std::vector<std::string> regexp_split(const std::string& str, std::string rgx_str)
{
  std::vector<std::string> lst;
  std::regex rgx(rgx_str);
  std::sregex_token_iterator iter(str.begin(), str.end(), rgx, -1);
  std::sregex_token_iterator end;

  while (iter != end)
  {
    lst.push_back(*iter);
    trim(lst.back());
    ++iter;
  }

  return lst;
}

//  Function that can be used to extract tokens using regexp.
std::vector<std::string> regexp_extract(const std::string& str, std::string rgx_str)
{
  std::vector<std::string> lst;
  std::regex rgx(rgx_str);
  std::sregex_token_iterator iter(str.begin(), str.end(), rgx, 0);
  std::sregex_token_iterator end;

  while (iter != end)
  {
    lst.push_back(*iter);
    trim(lst.back());
    ++iter;
  }
  return lst;
}

//  Function that can be used to replace text using regex.
std::string regexp_replace(const std::string& instr, const std::string& rgx_str, const std::string& replacement )
{
   std::regex re(rgx_str);
   std::string output_str = regex_replace(instr, re, replacement);
   return output_str;
}

//  Function that can be used to replace text.
std::string string_replace_all(const std::string& instr, const std::string& replace_from, const std::string& replace_to)
{
    auto output_str = instr;
    size_t pos = output_str.find(replace_from);
    while (pos != std::string::npos) {
        output_str.replace(pos, replace_from.size(), replace_to);
        pos = output_str.find(replace_from, pos + replace_to.size());
    }
    return output_str;
}

// Extract the string value from string
std::string regexp_extract_string( const std::string& regstr, const std::string& data )
{
    std::string token = "";
    std::regex re( regstr );
    std::smatch match;

    if( std::regex_search( data, match, re ))
    {
        if (match.ready())
            token = match[1];
    }
    return token;
}

// Extract the string value by key from jsonstring
std::string extract_string_json_old( const std::string& key, const std::string& jsondata )
{
    std::string data = jsondata;
    replace_all( data, "\'", '\"');
    std::string regstr =  std::string(".*\"")+key+"\"\\s*:\\s*\"([^\"]*)\".*";
    return regexp_extract_string( regstr, data );
}

// Extract the string value by key from query
int extract_int_json( const std::string& key, const std::string& jsondata )
{
    std::string data = jsondata;
    replace_all( data, "\'", '\"');
    std::string regstr =  std::string(".*\"")+key+"\"\\s*:\\s*([+-]?[1-9]\\d*|0).*";
    auto token = regexp_extract_string( regstr, data );
    if( token.empty() )
        return 0;
    return stoi(token);
}

// Extract the string value by key from jsonstring
std::string extract_string_json( const std::string& key, const std::string& jsondata )
{
    size_t key_size = key.length()+2;
    std::string key_find = "\"" + key + "\"";
    std::string field_value;

    auto pos_set = jsondata.find( key_find, 0);
    while( pos_set != std::string::npos )
    {
       pos_set += key_size;
       while(isspace( jsondata[pos_set] ) )
           ++pos_set;
       if( jsondata[pos_set] == ':')
       {
          ++pos_set;
          while( isspace( jsondata[pos_set] ) )
               ++pos_set;
          if( jsondata[pos_set] == '\"')
          {
            ++pos_set;
            auto pos_end = jsondata.find_first_of( "\"", pos_set);
            field_value =  jsondata.substr( pos_set, pos_end-pos_set);
            break;
          }
       }
       pos_set = jsondata.find( key_find, pos_set);
    }
    return field_value;
}

// How to split a string in C++.
//std::vector<std::string> split2(const std::string& s, char delimiter)
//{
//   std::vector<std::string> tokens;
//   std::string token;
//   std::istringstream tokenStream(s);
//   while (std::getline(tokenStream, token, delimiter))
//   {
//      tokens.push_back(token);
//   }
//   return tokens;
//}


std::vector<size_t> min_vector(const std::vector<size_t>& lhs, const std::vector<size_t>& rhs)
{
    std::vector<size_t> result;
    auto size = std::min(lhs.size(), rhs.size() );
    for( std::size_t ii=0; ii<size; ++ii )
    {
       result.push_back( std::min(lhs[ii], rhs[ii] ));
    }
    std::copy( rhs.begin(), rhs.end(), std::back_inserter(result) );
    return result;
}

/*

 Inserting several elements into an STL container efficiently


std::vector<int> v;
std::vector<int> newElements = {1, 3, 4, 2, -7, 8};

1.
   v.reserve("new_size");
   std::copy(begin(newElements), end(newElements), std::back_inserter(v));

2. For appending several new elements to an existing vector:
   v.insert(end(v), begin(newElements), end(newElements));
3. To replace the entire contents of a vector with newElements:
   v.assign(begin(newElements), end(newElements));
4.  At initialization of the vector, use the range constructor:
   std::vector<int> v{begin(newElements), end(newElements)};


*/

} // namespace jsonio17
