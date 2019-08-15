#include <fstream>
#include <regex>
#include "service.h"

// might be
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

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

// Get all  regular file names from the directory.
std::vector<std::string> files_into_directory( const std::string& directory_path, const std::string& sample )
{
    std::vector<std::string> fileNames;

    if( !directory_path.empty() )
    {
        fs::path ps(directory_path);

        if( fs::exists(ps ))
        {
            fs::directory_iterator end_itr;

            for(fs::directory_iterator itr(ps); itr != end_itr; ++itr)
            {
                if (fs::is_regular_file(itr->path()))
                {
                    std::string file = itr->path().string();
                    //cout << "file = " << file << endl;
                    if ( sample.empty() || file.find(sample) != std::string::npos)
                        fileNames.push_back(file);
                }
            }
        }
    }
    return fileNames;
}

// Read whole ASCII file into string.
std::string read_all_file( const std::string& file_path )
{
    std::ifstream t(file_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
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
