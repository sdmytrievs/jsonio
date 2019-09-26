#pragma once

#include "jsonbase.h"

namespace jsonio14 {


/// Get all regular file names from the directory.
std::vector<std::string> files_into_directory( const std::string& directory_path, const std::string& sample = "");

/// Read whole ASCII file into string.
std::string read_ascii_file( const std::string& file_path );


///  Base interface for working eith text files.
class TxtFile
{

public:

    /// Destructor
    virtual ~TxtFile()
    {}



protected:


};




} // namespace jsonio14
