#include <fstream>

#ifndef _WIN32
#include <pwd.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

#include "jsonio14/txt2file.h"
#include "jsonio14/jsonfree.h"

//#include "boost/filesystem.hpp"
//namespace fs = boost::filesystem;
// might be
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


namespace jsonio14 {


// Get home directory in Linux, C++
// https://stackoverflow.com/questions/3020187/getting-home-directory-in-mac-os-x-using-c-language
// This should work under Linux, Unix and OS X, for Windows you need to make a slight modification.
std::string  home_dir()
{
#ifdef _WIN32
    //const char *homeDir;
    //char homedir[1000];
    //snprintf(homedir, 1000, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
    //homeDir = getenv("HOMEDRIVE");
    std::string homeDir;
    auto current_path =  fs::current_path();
    homeDir = current_path.root_path().string();
    return homeDir;
#else
    const char *homeDir;
    homeDir = getenv("HOME");

    if( !homeDir )
    {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd)
            homeDir = pwd->pw_dir;
    }
    JARANGO_THROW_IF( !homeDir, "filesystem", 1,  "HOME environment variable not set.");
    //std::cout << "Home directory is " << homeDir << std::endl;
    return std::string(homeDir);
#endif
}

//  "~" generally refers to the user's home directory, this is solely an artifact of tilde expansion in Unix shells.
std::string expand_home_dir( const std::string& in_path, const std::string& in_home_dir )
{
    static std::string  aHomeDir(home_dir());

    if( in_path.size () > 0 && in_path[0] == '~')
        return (( in_home_dir.empty() ? aHomeDir: in_home_dir) + in_path.substr(1));
    else
        return in_path;
}



std::string make_path(const std::string& dir, const std::string& name, const std::string& ext)
{
    std::string Path(dir);
    if( dir != "")
        Path += "/";
    Path += name + "." + ext;
    return Path;
}

/// Creates the directory path.
bool create_directory( const std::string& path )
{
    fs::path ps(path);
    return fs::create_directories( ps );
}

/// Copies a directory, the subdirectories are also copied, with their content, recursively.
/// \param source - 	path to the source directory
///        target - 	path to the target directory
void copy_directory( const std::string& source,  const std::string& target )
{
    fs::path from(source), to(target);
    fs::copy(from, to, fs::copy_options::recursive);
}

/// Copies a single file.
/// \param source - 	path to the source file
///        target - 	path to the target file
void copy_file( const std::string& source,  const std::string& target )
{
    fs::path from(source), to(target);
    fs::copy_file(from, to, fs::copy_options::overwrite_existing);
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
            for(auto& p: fs::directory_iterator(ps))
            {
                if (fs::is_regular_file(p.path()))
                {
                    std::string file = p.path().string();
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
    JARANGO_THROW_IF( !t.good(), "filesystem", 4, "file open error...  " + file_path );
    std::stringstream buffer;
    buffer << t.rdbuf();

    auto retstr = buffer.str();
    // skip over optional BOM http://unicode.org/faq/utf_bom.html
    if ( retstr.size() >= 3 && static_cast<uint8_t>(retstr[0]) == 0xef &&
         static_cast<uint8_t>(retstr[1]) == 0xbb &&
        static_cast<uint8_t>(retstr[2]) == 0xbf )
    {
      // found UTF-8 BOM. simply skip over it
      retstr = retstr.substr(3);
    }
    return retstr;
}

bool path_exist(const std::string& path)
{
    fs::path ps(path);
    return fs::exists(ps);
}


//  TxtFile   ---------------------------------------------------------------------------

TxtFile::TxtFile( const std::string &fname, const std::string &fext, const std::string &fdir ):
    TxtFile( make_path( fdir, fname, fext) )
{ }

TxtFile::TxtFile(const std::string &fpath):
    file_path(fpath)
{
    fs::path ps(fpath);

    file_path = ps.string();
    file_dir =  ps.parent_path().string();
    file_name = ps.stem().string();
    file_ext = ps.extension().string();
    if( !file_ext.empty() )
      file_ext = file_ext.substr(1);
}

bool TxtFile::exist() const
{
    return fs::exists(file_path);
}

bool TxtFile::check_permission(TxtFile::OpenModeTypes mode) const
{
    bool bret = false;
    auto perms = fs::status(file_path).permissions();

    switch( mode )
    {
    case  ReadOnly:
        bret = (perms & fs::perms::others_read) != fs::perms::none;
        break;
    case  WriteOnly:
    case  Append:
        bret = (perms & fs::perms::owner_write) != fs::perms::none;
        break;
    case  ReadWrite:
        bret = (perms & fs::perms::others_read) != fs::perms::none &&
                (perms & fs::perms::owner_write) != fs::perms::none ;
        break;
    default:
        break;
    }
    return bret;
}

std::string TxtFile::load_all() const
{
    JARANGO_THROW_IF( !exist(), "filesystem", 2, "trying read not existing file  " + file_path );
    JARANGO_THROW_IF( !check_permission(ReadOnly), "filesystem", 3, "other users have not read permission  " + file_path );
    return read_ascii_file( file_path );
}

//  JsonFile   --------------------------------------------------------------

void JsonFile::load( JsonBase& object ) const
{
    if( file_type == Json )
        loadJson( object );
}

void JsonFile::save(const JsonBase& object) const
{
    saveJson(  object );
}

std::string JsonFile::load_json() const
{
    auto obj = JsonFree::object();
    load( obj );
    return obj.dump(true);
}

void JsonFile::loadJson(JsonBase& object) const
{
    auto jsonstr = TxtFile::load_all();
    object.loads(jsonstr);
}

void JsonFile::saveJson(const JsonBase& object) const
{
    std::fstream fout(file_path, std::ios::out );
    JARANGO_THROW_IF( !fout.good(), "filesystem", 5, "file save error...  " + file_path );
    object.dump( fout, false );
}

//  JsonArrayFile --------------------------------------------------------------------------


bool JsonArrayFile::loadNext( std::string &strjson )
{
    if( loaded_ndx >= arr_object.size() )
        return false;

    strjson = arr_object[loaded_ndx++].dump();
    return true;
}

bool JsonArrayFile::loadNext( JsonBase &object )
{
    std::string jsonstr;

    if( loadNext(jsonstr) )
    {
        object.loads(jsonstr);
        return true;
    }
    return false;
}

bool JsonArrayFile::saveNext(const std::string &strjson)
{
   loaded_ndx = arr_object.size();
 //  arr_object[ loaded_ndx ] = JsonFree::object(); //??? do we need
   arr_object[ loaded_ndx ].loads(strjson);
   return true;
}

bool JsonArrayFile::saveNext(const JsonBase &object)
{
  return saveNext( object.dump() );
}

void JsonArrayFile::Close()
{
    if( open_mode == WriteOnly )
    {
        save(arr_object);
    }
    is_opened = false;
}

void JsonArrayFile::Open(TxtFile::OpenModeTypes amode)
{
    if( isOpened() )
    {
        JARANGO_THROW_IF( open_mode!=amode, "filesystem", 6, "file was opened in different mode  " + file_path );
        return;
    }

    // clear settings
    open_mode  = amode;
    loaded_ndx = 0;
    arr_object.clear();

    if( open_mode == ReadOnly )
    {
        load( arr_object );
    }
    else if( open_mode != WriteOnly )
    {
        JARANGO_THROW( "filesystem", 7, "illegal file open mode for class JsonArrayFile" );
    }
    is_opened = true;
}


} // namespace jsonio14
