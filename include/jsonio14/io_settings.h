#pragma once

#include <iostream>
#include <vector>
#include "jsonio14/schema.h"

namespace jsonio14 {

class JsonioSettings;
/// Function to connect for a Preferences object
extern JsonioSettings& ioSettings();


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
    std::string directoryPath( const std::string& jsonpath, const std::string& defvalue  ) const;

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
    bool setValue( const std::string& jsonpath, const T& value  )
    {
        if( head_object->set_value_via_path( jsonpath, value ) )
        {
            sync();
            return true;
        }
        return false;
    }

    bool setValue( const std::string& jsonpath, const char* value  )
    {
        return setValue( jsonpath, std::string(value) );
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

    /// Constructor
    explicit JsonioSettings( const std::string& config_file_path );

    JsonioSettings( const JsonioSettings& other ) =default;
    JsonioSettings( JsonioSettings&& other ) =default;

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
    std::string directoryPath( const std::string& jsonpath, const std::string& defvalue  ) const
    {
        return top_section.directoryPath( jsonpath, defvalue);
    }

    /// Get value from settings
    template <class T>
    T value(const std::string& jsonpath, const T& defaultValue ) const
    {
        return top_section.value( jsonpath, defaultValue);
    }

    /// Set addition settings
    template <class T>
    void setValue( const char *jsonpath, const T& avalue )
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

private:

    /// Internal Data File
    JsonFile config_file;

    /// Properties for program
    JsonFree all_settings;

    /// Link to top settings
    SectionSettings top_section;

    std::string jsonio_section_name = "jsonio";

    /// Link to jsonio settings
    SectionSettings jsonio_section;

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

};


} // namespace jsonio14

