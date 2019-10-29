
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonio14/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Create the shared library using the collected source files
add_library(JSONIO14_OBJECTS OBJECT ${HEADER_FILES} ${SOURCE_FILES} )

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    add_library(JSONIO14_SHARED SHARED $<TARGET_OBJECTS:JSONIO14_OBJECTS>)
#if( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
#    target_link_libraries(JSONARANGO_SHARED curl velocypack )
#endif()
    set_target_properties(JSONIO14_SHARED PROPERTIES OUTPUT_NAME jsonio14)
    install(TARGETS JSONIO14_SHARED DESTINATION lib)
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    add_library(JSONIO14_STATIC STATIC $<TARGET_OBJECTS:JSONIO14_OBJECTS>)
    set_target_properties(JSONIO14_STATIC PROPERTIES OUTPUT_NAME jsonio14)
    install(TARGETS JSONIO14_STATIC DESTINATION lib)
endif()


install(
    DIRECTORY   "${JSONIO14_HEADER_DIR}/jsonio14"
    DESTINATION include
)
                  
