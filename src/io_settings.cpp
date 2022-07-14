#include "jsonio17/io_settings.h"
#include "jsonio17/schema_thrift.h"
#include "jsonio17/dbconnect.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>


namespace jsonio17 {

const std::string version = "v1.0";
std::string JsonioSettings::settingsFileName = "jsonio17-config.json";
std::string JsonioSettings::jsonio_section_name = "jsonio";
std::string JsonioSettings::logger_section_name = "log";


JsonioSettings& ioSettings()
{
    static  JsonioSettings data(JsonioSettings::settingsFileName);
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

JsonioSettings::JsonioSettings(const std::string& config_file_path):
    config_file(config_file_path), all_settings(JsonFree::object()),
    top_section( SectionSettings(*this, &all_settings)),
    jsonio_section(SectionSettings(*this, &all_settings)),
    logger_section(SectionSettings(*this, &all_settings)),
    schema()
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
    updateLogger();
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
        for (const auto& file: file_names)
            schema.addSchemaFile( schema_thrift, file );
    }
    // init vertex&edge lists from schema data
    DataBase::update_from_schema( schema.allStructs() );
}

std::shared_ptr<spdlog::logger> JsonioSettings::get_logger(const std::string &module)
{
  auto pattern = logger_section.value<std::string>("pattern", "[%n] [%^%l%$] %v");
  auto linked_it = linked_logger_names.find(module);
  if( linked_it == linked_logger_names.end() ) {
     linked_logger_names.insert(module);
     logger_section.setValue<std::set<std::string>>("modules", linked_logger_names );
  }
  auto res = spdlog::get(module);
  if (!res) {
      res = spdlog::stdout_color_mt(module);
      res->set_level(get_level(module));
  }
  res->set_pattern(pattern);
  module_loggers.insert(res);
  io_logger->debug("add logger {} {}", res->name(), module_loggers.size());
  return res;
}

void JsonioSettings::set_levels(const std::string &level_name)
{
    logger_section.setValue<std::string>("level",level_name);
    for(const auto& logger: module_loggers) {
        logger->set_level(get_level(logger->name()));
    }
}

void JsonioSettings::set_pattern(const std::string &pattern)
{
    logger_section.setValue<std::string>("pattern",pattern);
    for(const auto& logger: module_loggers) {
        logger->set_pattern(pattern);
    }
}

void JsonioSettings::set_module_level(const std::string &module_name, const std::string &level_name)
{
    auto module_logger = get_logger(module_name);
    if(module_logger) {
        module_logger->set_level(spdlog::level::from_str(level_name));
    }
    logger_section.setValue<std::string>("module_level."+module_name, level_name);
}

spdlog::level::level_enum JsonioSettings::get_level(const std::string &module_name)
{
    auto global_name = logger_section.value<std::string>("level", "info");
    auto level_name = logger_section.value<std::string>("module_level."+module_name, global_name);
    return spdlog::level::from_str(level_name);
}

bool JsonioSettings::updateLogger()
{
    logger_section.change_head(&all_settings.add_object_via_path(logger_section_name));
    auto module_names = logger_section.value<std::vector<std::string>>("modules", {});
    if(module_names.empty() ) {
        get_logger("jsonio17");
        get_logger("jsonarango");
    }
    else {
        linked_logger_names.clear();
        module_loggers.clear();
        linked_logger_names.insert(module_names.begin(), module_names.end());
        for( const auto& module_name: linked_logger_names) {
            get_logger(module_name);
        }
        // set file loggers
        add_file_sinks();
    }
    set_levels(logger_section.value<std::string>("level","info"));
    return true;
}

void JsonioSettings::add_file_sinks()
{
    if(!logger_section.contains("file")) {
        return;
    }
    auto file_logger_section = section(logger_section_name+".file");
    auto file_module_names = file_logger_section.value<std::vector<std::string>>("modules", {});
    if(file_module_names.empty()) {
        return;
    }
    auto logfile_path = file_logger_section.value<std::string>("path", "log.txt");
    auto logfile_size = file_logger_section.value<size_t>("size", 1048576);
    auto logfile_count = file_logger_section.value<size_t>("count", 3);
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    logfile_path, logfile_size, logfile_count);

    for(const auto& module_name: file_module_names) {
        auto plogger = spdlog::get(module_name);
        if (plogger) {
            plogger->sinks().push_back(file_sink);
        }
    }
}

} // namespace jsonio
