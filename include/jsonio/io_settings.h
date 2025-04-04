#pragma once

#include <vector>
#include "jsonio/schema.h"

namespace jsonio {

class JsonioSettings;
/// Function to connect for a Preferences object
extern JsonioSettings &ioSettings();


/// Definition of json based section settings
class SectionSettings final
{

public:

    /// Check if a section object contains a certain jsonpath.
    bool contains(const std::string& jsonpath ) const
    {
        return head_object->path_if_exists( jsonpath );
    }

    /// @brief Dump section to JSON string.
    std::string dump() const
    {
        return  head_object->dump( true );
    }

    /// Get Directory path from Node
    /// "~" refers to the "UserHomeDirectoryPath" directory
    std::string directoryPath( const std::string& jsonpath, const std::string& defvalue ) const;

    /// Explicit type conversion between the JSON path value and a compatible value.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// The value is filled into the input parameter.
    /// @return true if JSON value exist and can be converted to value type.
    template <class T>
    T value( const std::string& jsonpath, const T& defvalue  ) const
    {
        T avalue;
        head_object->get_value_via_path( jsonpath, avalue, defvalue );
        return avalue;
    }

    /// Use jsonpath to modify any value in a JSON object.
    /// The following jsonpath expression could be used
    ///     "name1.name2.3.name3"
    ///     "name1.name2[3].name3"
    ///     "/name1/name2/3/name3"
    ///     "/name1/name2[3]/name3"
    ///     "[\"name1\"][\"name2\"][3][\"name3\"]"
    /// @return true if modified.
    template <class T>
    bool setValue( const std::string& jsonpath, const T& avalue  )
    {
        if( head_object->set_value_via_path( jsonpath, avalue ) )
        {
            sync();
            return true;
        }
        return false;
    }

    bool setValue( const std::string& jsonpath, const char* avalue  )
    {
        return setValue( jsonpath, std::string(avalue) );
    }

protected:

    /// Constructor
    SectionSettings( const JsonioSettings& iosettins,
                     JsonFree* topData ):
        io_settins( iosettins ), head_object( topData )
    { }

    /// Full configuration data.
    const JsonioSettings& io_settins;

    /// Head object for section.
    JsonFree* head_object;

    void change_head( JsonFree* new_head_object )
    {
        head_object = new_head_object;
    }

    void sync() const;

    friend class JsonioSettings;
};


/// \class JsonioSettings - storing preferences to JSONIO
class JsonioSettings
{

public:

    /// Task settings file name
    static std::string settingsFileName;
    static std::string jsonio_section_name;
    static std::string logger_section_name;

    /// Path to logger data (by default empty)
    static std::string data_logger_directory;
    static size_t log_file_size;
    static size_t log_file_count;

    /// Constructor
    explicit JsonioSettings( const std::string& config_file_path );
    virtual ~JsonioSettings();

    /// Get section from settings
    SectionSettings section( const std::string& section_jsonpath )
    {
        JsonFree& data = all_settings.add_object_via_path(section_jsonpath);
        return SectionSettings( *this, &data );
    }

    /// Returns true if there exists a setting called key; returns false otherwise.
    bool contains(const std::string& jsonpath ) const
    {
        return top_section.contains( jsonpath );
    }

    /// Get Directory path from Node
    /// "~" refers to the "UserHomeDirectoryPath" directory
    std::string directoryPath( const std::string& jsonpath, const std::string& defvalue ) const
    {
        return top_section.directoryPath( jsonpath, defvalue);
    }

    /// Get value from settings
    template <class T>
    T value( const std::string& jsonpath, const T& defaultValue ) const
    {
        return top_section.value( jsonpath, defaultValue);
    }

    /// Set addition settings
    template <class T>
    void setValue( const std::string& jsonpath, const T& avalue )
    {
        top_section.setValue( jsonpath, avalue);
    }

    /// @brief Dump settings to JSON string.
    std::string dump() const
    {
        return  all_settings.dump( true );
    }

    /// Writes any unsaved changes to permanent storage
    virtual void sync() const
    {
        config_file.save( all_settings );
    }

    // jsonio internal data links ------------------------------

    /// Current work directory
    std::string userDir() const
    {
        return UserDir;
    }

    /// Update user directory
    virtual void setUserDir( const std::string& dirPath );

    /// Current work directory
    std::string homeDir() const
    {
        return HomeDir;
    }

    /// Update user directory
    virtual void setHomeDir( const std::string& dirPath );

    /// Path to Resources files
    std::string resourcesDir() const
    {
        return directoryPath( "common.ResourcesDirectory",  std::string("") );
    }

    /// Link to schemas collection
    const SchemasData&  Schema() const
    {
        return schema;
    }

    /// Read schema description from json string
    void addSchemaFormat( const std::string& format_type, const std::string& json_string );

    /// Update/reread schema directory
    bool updateSchemaDir();

    /// Creates or fetches an spdlog logger.
    /// @param module Name of the logger.
    std::shared_ptr<spdlog::logger> get_logger(const std::string& module);

    /// Set the output level for all existing and future loggers.
    /// @param level_name     the level to set
    void set_levels(const std::string& level_name);

    /// Configure the output levels for separate module.
    /// @param  module and the level to set
    void set_module_level( const std::string& module_name, const std::string& level_name);

    /// Set the output pattern for all existing and future loggers.
    /// @param pattern     the pattern to set
    void set_pattern(const std::string &pattern);

    void addSchemaFile(const std::string &file_path);
private:

    /// Internal Data File
    JsonFile config_file;

    /// Properties for program
    JsonFree all_settings;

    /// Link to top settings
    SectionSettings top_section;

    /// Link to jsonio settings
    SectionSettings jsonio_section;

    /// Link to spdlog settings
    SectionSettings logger_section;

    // jsonio internal data --------------------------------

    /// Current work directory
    std::string UserDir = "";
    /// Current home directory
    /// "~" generally refers to the user's home directory
    /// Would be used to update directory path containing "~"
    std::string HomeDir = ".";

    /// Path to schemas directory
    std::string SchemDir ="";
    /// Current schema structure
    SchemasData schema;

    /// Read data from settings
    virtual void getDataFromPreferences();

    /// Read all schemas from Directory
    void readSchemaDir( const std::string& dir_path );

    // logger internal data --------------------------------

    std::set<std::shared_ptr<spdlog::logger>> module_loggers;
    std::set<std::string> linked_logger_names;

    /// Get the output level for module logger.
    /// @param module_name Name of the logger.
    spdlog::level::level_enum get_level(const std::string& module);
    void add_file_sinks();

    /// Update/reread spdlog settings
    bool update_logger();
    std::string with_directory(const std::string &logfile_name);
};

inline std::string common_section( const std::string& item )
{
    return   "common."+item;
}

inline std::string jsonio_section( const std::string& item )
{
    return   jsonio::JsonioSettings::jsonio_section_name+"."+item;
}

inline std::string arangodb_section( const std::string& item )
{
    return   "arangodb."+item;
}

} // namespace jsonio

