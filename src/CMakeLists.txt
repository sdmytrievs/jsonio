
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonio/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Create the shared library using the collected source files
#add_library(JSONIO_OBJECTS OBJECT ${HEADER_FILES} ${SOURCE_FILES} )

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    # Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    #add_library(jsonio SHARED $<TARGET_OBJECTS:JSONIO_OBJECTS>)
    add_library(jsonio SHARED ${HEADER_FILES} ${SOURCE_FILES})
    add_library(jsonio::jsonio ALIAS jsonio)
    target_link_libraries(jsonio PUBLIC arango-cpp::arango-cpp)

    install( TARGETS jsonio
         EXPORT jsonioTargets DESTINATION "lib" COMPONENT libraries)

    # Install debug symbols
    if(MSVC)
       install(
             FILES $<TARGET_PDB_FILE:jsonio>
             DESTINATION ${CMAKE_INSTALL_BINDIR}
             COMPONENT libraries
             OPTIONAL)
    endif()
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    #add_library(jsonio-static STATIC  $<TARGET_OBJECTS:JSONIO_OBJECTS>)
    add_library(jsonio-static STATIC ${HEADER_FILES} ${SOURCE_FILES})
    add_library(jsonio::jsonio-static ALIAS jsonio-static)
    target_link_libraries(jsonio-static PUBLIC arango-cpp::arango-cpp-static)

    install( TARGETS jsonio-static
            EXPORT jsonioTargets DESTINATION "lib" COMPONENT libraries)
endif()

install(
    DIRECTORY   "${JSONIO_HEADER_DIR}/jsonio"
    DESTINATION include
)
                  
