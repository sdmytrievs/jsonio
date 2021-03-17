# Require a certain version of cmake
cmake_minimum_required(VERSION 3.7)

# Set the name of the project
project(jsonio17 CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH ON)

# Include the cmake variables with values for installation directories
include(GNUInstallDirs)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
include(CCache)

# Ensure proper configuration if in a conda environment
include(CondaAware)


# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


# Define which types of libraries to build
option(BUILD_SHARED_LIBS "Build shared libraries." ON)
option(BUILD_STATIC_LIBS "Build static libraries." ON)
option(BuildExamples "Build EXAMPLES" ON)
option(BuildTests "Build test suite" ON)

# Currently is not setup to produce a dynamic library using MSVC, only static
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(BUILD_SHARED_LIBS OFF)
    set(BuildExamples OFF)
    set(BuildTests OFF)
endif()

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options(-std=c++17 -Wall -Wno-pedantic -Wno-variadic-macros -Wno-deprecated )
endif()

# Set the list of compiler flags for Clang compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-std=c++17 -Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Intel compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    add_compile_options(-std=c++17 -Wall -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(
        /D_SCL_SECURE_NO_WARNINGS
        /D_CRT_SECURE_NO_WARNINGS=1
        /MP4
        /EHsc
        /Od
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()


# Set the jsonio17 source directory path
set(JSONIO17_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(JSONIO17_HEADER_DIR ${CMAKE_SOURCE_DIR}/include)


# Set the include directories
include_directories(${JSONIO17_SOURCE_DIR})
include_directories(${JSONIO17_HEADER_DIR})
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    include_directories("c:/usr/local/include")
    link_directories("c:/usr/local/lib")
endif()

# Find all  dependencies
include(JsonioFindDeps)

# Build jsonio library
add_subdirectory(src)
link_directories(${CMAKE_BINARY_DIR}/src)

# Build the examples
if(BuildExamples)
    add_subdirectory(examples)
endif()


# Run the tests
if(BuildTests)
    add_subdirectory(tests)
endif()

include(CPack) #should be last command

