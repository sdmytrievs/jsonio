#include "jsonio14/io_settings.h"
#include "jsonio14/schema_thrift.h"

namespace jsonio14 {

const std::string version = "v1.0";
//const std::string groupName = "jsonio";
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
    // register thrift schemas
    schema.addSchemaMethod( schema_thrift, ThriftSchemaRead );

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
    SchemDir =  "";
    updateSchemaDir();
}

// Connect localDB to new path
bool JsonioSettings::updateSchemaDir()
{
  std::string newSchema = directoryPath("common.SchemasDirectory", std::string(""));
  if( newSchema != SchemDir )
  {
    SchemDir  = newSchema;
    readSchemaDir( SchemDir );
    return true;
  }
  return false;
}

void JsonioSettings::setUserDir( const std::string& dir_path)
{
    if( dir_path.empty() )
        return;
    setValue( "common.WorkDirectoryPath", dir_path );
    UserDir = directoryPath( "common.WorkDirectoryPath", std::string(".") );
}

void JsonioSettings::setHomeDir( const std::string& dir_path )
{
    setValue( "common.UserHomeDirectoryPath", dir_path );
    HomeDir = value( "common.UserHomeDirectoryPath", std::string("") );
    HomeDir = expand_home_dir( HomeDir, "" ); // "~" or empty generally refers to the user's home directory
}

void JsonioSettings::addSchemaFormat(const std::string &format_type, const std::string &json_string)
{
    schema.addSchemaFormat(format_type, json_string);
}

void JsonioSettings::readSchemaDir(const std::string &dir_path)
{
    // Get all regular file names from the directory.
    auto file_names = files_into_directory( dir_path, ".schema.json" );

    if( !file_names.empty() )
    {
        schema.clear_all();
        for (auto file: file_names)
            schema.addSchemaFile( schema_thrift, file );
    }
}


} // namespace jsonio
