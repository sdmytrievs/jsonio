
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonio17/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Create the shared library using the collected source files
add_library(JSONIO17_OBJECTS OBJECT ${HEADER_FILES} ${SOURCE_FILES} )

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    add_library(JSONIO17_SHARED SHARED $<TARGET_OBJECTS:JSONIO17_OBJECTS>)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
      target_link_libraries(JSONIO17_SHARED
            PUBLIC stdc++fs
            PUBLIC jsonarango-static
            PRIVATE velocypack
            PRIVATE libcurl
            PUBLIC pthread )
else()
      target_link_libraries( JSONIO17_SHARED
           PUBLIC stdc++fs
           PUBLIC jsonarango
           PUBLIC velocypack
           PUBLIC curl
           PUBLIC pthread )
endif()

    set_target_properties(JSONIO17_SHARED PROPERTIES OUTPUT_NAME jsonio17)
    install(TARGETS JSONIO17_SHARED DESTINATION lib)
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    add_library(JSONIO17_STATIC STATIC $<TARGET_OBJECTS:JSONIO17_OBJECTS>)
    set_target_properties(JSONIO17_STATIC PROPERTIES OUTPUT_NAME jsonio17-static)
    install(TARGETS JSONIO17_STATIC DESTINATION lib)
endif()


install(
    DIRECTORY   "${JSONIO17_HEADER_DIR}/jsonio17"
    DESTINATION include
)
                  
