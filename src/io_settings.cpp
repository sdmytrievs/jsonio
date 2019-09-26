
#include "io_settings.h"

#ifndef _WIN32

#include <pwd.h>
#include <unistd.h>

#else

#include <unistd.h>

#endif


namespace jsonio14 {

const std::string version = "v1.0";
const std::string groupName = "jsonio";
std::string JsonioSettings::settingsFileName = "jsonio14-config.json";

JsonioSettings& ioSettings()
{
    static  JsonioSettings data( JsonioSettings::settingsFileName );
    return  data;
}

/// Get home directory in Linux, Unix and OS X, C++
std::string  getHomeDir();
///  Link path to home directory
///  "~" generally refers to the user's home directory.
std::string expandHomeDir( const std::string& inPath, const std::string& homeDir = "" );


// Get home directory in Linux, C++
// https://stackoverflow.com/questions/3020187/getting-home-directory-in-mac-os-x-using-c-language
// This should work under Linux, Unix and OS X, for Windows you need to make a slight modification.
std::string  getHomeDir()
{
    const char *homeDir;
#ifdef _WIN32
    //char homedir[MAX_PATH];
    //snprintf(homedir, MAX_PATH, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
    homeDir =  getenv("HOMEDRIVE");
#else
    homeDir = getenv("HOME");

    if( !homeDir )
    {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd)
            homeDir = pwd->pw_dir;
    }
#endif
    JARANGO_THROW_IF( !homeDir, "JsonioSettings", 1,  "HOME environment variable not set.");
    //std::cout << "Home directory is " << homeDir << std::endl;
    return std::string(homeDir);
}

//  "~" generally refers to the user's home directory, this is solely an artifact of tilde expansion in Unix shells.
std::string expandHomeDir( const std::string& inPath, const std::string& homeDir )
{
    static std::string  aHomeDir(getHomeDir());

    if( inPath.size () > 0 && inPath[0] == '~')
        return (( homeDir.empty() ? aHomeDir: homeDir) + inPath.substr(1));
    else
        return inPath;
}


void SectionSettings::sync() const
{
    io_settins.sync();
}

std::string SectionSettings::directoryPath( const std::string& fldpath, const std::string& defvalue  ) const
{
    std::string dirPath = value(fldpath, defvalue );
    return expandHomeDir( dirPath, io_settins.homeDir() );
}

//------------------------------------------------------------

JsonioSettings::JsonioSettings( const std::string& /*config_file_path*/ ):
    /*config_file( config_file_path ),*/ all_settings(JsonFree::object()),
    top_section( SectionSettings( *this, &all_settings ) ),
    jsonio_section( SectionSettings( *this, &all_settings ) )
{
    getDataFromPreferences();
}

JsonioSettings::~JsonioSettings()
{
    //        sync(); Temporarily disabled by DK on 3.07.19 to test abnormal behavior of GEMSW

}

void JsonioSettings::getDataFromPreferences()
{
    //if( configuration_file.exist() )
    //      configuration_file.load( all_settings );
    JsonFree& data = all_settings.add_object_via_path(jsonio_section_name);
    jsonio_section.change_head( &data );
    HomeDir = value( "common.UserHomeDirectoryPath", std::string("") );
    HomeDir = expandHomeDir( HomeDir, "" ); // "~" or empty generally refers to the user's home directory
    UserDir = directoryPath( "common.WorkDirectoryPath", std::string(".") );
}

void JsonioSettings::setUserDir( const std::string& dirPath)
{
    if( dirPath.empty() )
        return;
    setValue( "common.WorkDirectoryPath", dirPath );
    UserDir = directoryPath( "common.WorkDirectoryPath", std::string(".") );
}

void JsonioSettings::setHomeDir( const std::string& dirPath )
{
    setValue( "common.UserHomeDirectoryPath", dirPath );
    HomeDir = value( "common.UserHomeDirectoryPath", std::string("") );
    HomeDir = expandHomeDir( HomeDir, "" ); // "~" or empty generally refers to the user's home directory
}


} // namespace jsonio
