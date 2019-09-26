#include <fstream>

#include "txt2file.h"

#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

// might be
// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;


namespace jsonio14 {


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
std::string read_ascii_file( const std::string& file_path )
{
    std::ifstream t(file_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

/* to do


/// Creates the directory path.
bool create_directory( const std::string& path );

/// Copies a directory, the subdirectories are also copied, with their content, recursively.
/// \param source - 	path to the source directory
///        target - 	path to the target directory
void copy_directory( const std::string& source,  const std::string& target );

/// Copies a single file.
/// \param source - 	path to the source file
///        target - 	path to the target file
void copy_file( const std::string& source,  const std::string& target );


*/

} // namespace jsonio14
