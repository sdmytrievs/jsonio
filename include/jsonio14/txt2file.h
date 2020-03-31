#pragma once

#include "jsonio14/jsonfree.h"

namespace jsonio14 {


/// Get all regular file names from the directory.
list_names_t files_into_directory( const std::string& directory_path, const std::string& sample = "");

/// Read whole ASCII file into string.
std::string read_ascii_file( const std::string& file_path );

/// Checks if the given file status or path corresponds to an existing file or directory.
bool path_exist( const std::string& path );

/// Get home directory in Linux, Unix and OS X, C++
std::string  home_dir();
///  Link path to home directory
///  "~" generally refers to the user's home directory.
std::string expand_home_dir( const std::string& path, const std::string& home_dir = "" );


///  Base interface for working with text files.
class TxtFile
{

public:

    /// Possible text file formats.
    enum FileTypes {
        Txt = 't', Json = 'j', Yaml = 'y',  XML = 'x'
    };

    /// This enum is used to describe the mode in which a device is opened
    enum OpenModeTypes {
        NotOpen,       ///<	The device is not open.
        ReadOnly,      ///<	The device is open for reading.
        WriteOnly, 	   ///< The device is open for writing.
        ReadWrite,     ///<	ReadOnly | WriteOnly	The device is open for reading and writing.
        Append	       ///<	The device is opened in append mode so that all data is written to the end of the file.
    };

    /// Constructor
    TxtFile(const std::string& name, const std::string& ext, const std::string& dir="" );
    /// Constructor from path
    TxtFile(const std::string& fpath);

    /// Destructor
    virtual ~TxtFile()
    {}

    /// Copy constructor
    TxtFile(const TxtFile &obj ) = default;
    /// Move constructor
    TxtFile( TxtFile &&obj ) = default;
    /// Copy assignment
    TxtFile &operator =( const TxtFile &other) = default;
    /// Move assignment
    TxtFile &operator =( TxtFile &&other) = default;

    /// Checks if the given path corresponds to an existing file.
    bool exist() const;
    /// Check file access permissions for mode ( Other users have read/write permission ).
    bool check_permission(OpenModeTypes  mode) const;

    bool isOpened()
    { return is_opened; }

    const std::string&  name() const
    { return file_name;  }
    const std::string&  ext() const
    { return file_ext;    }
    const std::string&  dir() const
    { return file_dir;    }
    const std::string&  path() const
    { return file_path;  }
    FileTypes type() const
    { return file_type;  }


    /// Read whole file into string.
    virtual std::string load_all() const;

protected:

    std::string file_dir = "";
    std::string file_name = "";
    std::string file_ext = "";
    std::string file_path;

    FileTypes file_type = Txt;
    OpenModeTypes  open_mode = ReadWrite;
    bool  is_opened = false;

};

/// Class for read/write json files
class JsonFile : public TxtFile
{

public:

    /// Constructor
    JsonFile( const std::string& name, const std::string& ext, const std::string& dir=""):
        TxtFile( name, ext, dir )
    {
        file_type =  Json;
    }

    /// Constructor from path
    JsonFile( const std::string& path): TxtFile( path )
    {
        file_type =  Json;
    }

    /// Load data from file to json object
    virtual void load( JsonBase& object ) const;

    /// Save data from json object to file
    virtual void save( const JsonBase& object ) const;

    /// Convert readed data to json string
    std::string load_json() const;

protected:

    /// Load data from json file to dom object
    void loadJson( JsonBase& object ) const;

    /// Save data from dom object to  json file
    void saveJson( const JsonBase& object ) const ;

};


/// Class for read/write json arrays files
class JsonArrayFile : public JsonFile
{

public:

    /// Constructor
    JsonArrayFile( const std::string& name, const std::string& ext, const std::string& dir=""):
        JsonFile( name, ext, dir ), arr_object( JsonFree::array() )
    { }
    /// Constructor from path
    JsonArrayFile( const std::string& path):
        JsonFile( path ), arr_object( JsonFree::array() )
    { }
    /// Destructor
    virtual ~JsonArrayFile()
    {
        if( is_opened )
            Close();
    }

    /// Copy constructor
    JsonArrayFile(const JsonArrayFile &obj ) = default;
    /// Move constructor
    JsonArrayFile( JsonArrayFile &&obj ) = default;
    /// Copy assignment
    JsonArrayFile &operator =( const JsonArrayFile &other) = default;
    /// Move assignment
    JsonArrayFile &operator =( JsonArrayFile &&other) = default;

    /// Load next object data from file to dom object
    bool loadNext( JsonBase&  object );

    /// Save next dom object to file
    bool saveNext( const JsonBase&  object );

    /// Load next object data from file to json string
    bool loadNext( std::string& strjson );

    /// Save next json string to file
    bool saveNext( const std::string& strjson );

    virtual void Close();
    virtual void Open(OpenModeTypes amode );

protected:

    std::size_t loaded_ndx = 0;   ///< Current loaded
    JsonFree arr_object;

};


} // namespace jsonio14
