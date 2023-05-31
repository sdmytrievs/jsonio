
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonio17/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Create the shared library using the collected source files
#add_library(JSONIO17_OBJECTS OBJECT ${HEADER_FILES} ${SOURCE_FILES} )

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    # Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    #add_library(jsonio17 SHARED $<TARGET_OBJECTS:JSONIO17_OBJECTS>)
    add_library(jsonio17 SHARED ${HEADER_FILES} ${SOURCE_FILES})
    add_library(jsonio17::jsonio17 ALIAS jsonio17)
    target_link_libraries(jsonio17 PUBLIC jsonarango::jsonarango)

    install( TARGETS jsonio17
         EXPORT jsonioTargets DESTINATION "lib" COMPONENT libraries)

    # Install debug symbols
    if(MSVC)
       install(
             FILES $<TARGET_PDB_FILE:jsonio17>
             DESTINATION ${CMAKE_INSTALL_BINDIR}
             COMPONENT libraries
             OPTIONAL)
    endif()
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    #add_library(jsonio17-static STATIC  $<TARGET_OBJECTS:JSONIO17_OBJECTS>)
    add_library(jsonio17-static STATIC ${HEADER_FILES} ${SOURCE_FILES})
    add_library(jsonio17::jsonio17-static ALIAS jsonio17-static)
    target_link_libraries(jsonio17-static PUBLIC jsonarango::jsonarango-static)

    install( TARGETS jsonio17-static
            EXPORT jsonioTargets DESTINATION "lib" COMPONENT libraries)
endif()

install(
    DIRECTORY   "${JSONIO17_HEADER_DIR}/jsonio17"
    DESTINATION include
)
                  
