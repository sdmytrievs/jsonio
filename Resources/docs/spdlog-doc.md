
##  using [spdlog](https://github.com/gabime/spdlog/tree/master) 

Very fast, header only, C++ logging library. 

source https://github.com/gabime/spdlog/tree/master

documentation https://spdlog.docsforge.com/master/

### Install spdlog

Header only version (default) 

```sh
        git clone https://github.com/gabime/spdlog -b v1.11.0  && \
        cd spdlog/include && \
        sudo cp -r spdlog /usr/local/include

```

Compiled version ( must build with ```cmake .. -DUSE_SPDLOG_PRECOMPILED=ON```)

* Ubuntu: apt-get install libspdlog-dev

* Homebrew: brew install spdlog

* Building spdlog library


### Brief

spdlog levels :  trace = 0, debug = 1, info = 2, warn = 3, err = 4, critical = 5, off = 6


```c++

inline spdlog::level::level_enum from_str(const std::string &name)
{
    static std::unordered_map<std::string, level_enum> name_to_level = // map string->level
        {{level_names[0], level::trace},                               // trace
            {level_names[1], level::debug},                            // debug
            {level_names[2], level::info},                             // info
            {level_names[3], level::warn},                             // warn
            {level_names[4], level::err},                              // err
            {level_names[5], level::critical},                         // critical
            {level_names[6], level::off}};                             // off

    auto lvl_it = name_to_level.find(name);
    return lvl_it != name_to_level.end() ? lvl_it->second : level::off;
}

```


### Used loggers into code

 Thread-safe logger to stdout with colors and/or to file loggers
 
 Currently, such loggers are implemented for jsonio17 and additional for jsonarango, jsonimpex17, and jsonui17. It is possible to find loggers by name and change the level. 

 All file loggers write to the working directory, but this can be fixed by changing JsonioSettings::data_logger_directory. 

```c++

std::shared_ptr<spdlog::logger> arango_logger = spdlog::stdout_color_mt("jsonarango");

std::shared_ptr<spdlog::logger> io_logger = spdlog::stdout_color_mt("jsonio17");

```

### API for changing loggers settings in source code

class JsonioSettings - storing preferences to JSONIO section "log"

```c++

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

```

Example 

```c++

ioSettings().set_levels("critical");

```

### Using configuration file (*jsonio17-config.json*) for changing loggers settings without rebuilding

* **log.level**     - level for all loggers except defined into log.module_level 
* **log.module_level**  - specific level for particular logger
* **log.modules**   - list of existing loggers
* **log.file**  - description of rotating file sink based on size 
* **log.file.modules**  - list of loggers to file

* **log.pattern** - pattern for loggers  (by default *"[%n] [%^%l%$] %v"*)
* **log.logs-directory** - all file loggers write to the directory


```json

{
"log" :   {
     "modules" :   [
          "jsonarango",
          "jsonimpex17",
          "jsonio17",
          "jsonui17"
     ],
     "level" :   "critical",
     "module_level" :   {
          "jsonarango" :   "info",
          "jsonimpex17" :   "off"
     },
     "file" :   {
          "path" :   "log.txt",
          "size" :   10000000,
          "count" :   10,
          "modules" :   [
               "jsonarango",
               "jsonio17"
          ]
     }
}


```

