#include "io_settings.h"

namespace jsonio14 {

const std::string version = "v1.0";
const std::string groupName = "jsonio";
std::string JsonioSettings::settingsFileName = "jsonio14-config.json";

JsonioSettings& ioSettings()
{
    static  JsonioSettings data( JsonioSettings::settingsFileName );
    return  data;
}


void SectionSettings::sync() const
{
    io_settins.sync();
}

std::string SectionSettings::directoryPath( const std::string& fldpath, const std::string& defvalue  ) const
{
    std::string dirPath = value(fldpath, defvalue );
    return expand_home_dir( dirPath, io_settins.homeDir() );
}

//------------------------------------------------------------

JsonioSettings::JsonioSettings( const std::string& config_file_path ):
    config_file( config_file_path ), all_settings(JsonFree::object()),
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
    if( config_file.exist() )
        config_file.load( all_settings );
    JsonFree& data = all_settings.add_object_via_path(jsonio_section_name);
    jsonio_section.change_head( &data );
    HomeDir = value( "common.UserHomeDirectoryPath", std::string("") );
    HomeDir = expand_home_dir( HomeDir, "" ); // "~" or empty generally refers to the user's home directory
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
    HomeDir = expand_home_dir( HomeDir, "" ); // "~" or empty generally refers to the user's home directory
}


} // namespace jsonio
