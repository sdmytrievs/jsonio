#include <regex>
#include "service.h"


namespace jsonio14 {

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


// How to split a string in C++.
std::vector<std::string> split2(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}


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

} // namespace jsonio14
