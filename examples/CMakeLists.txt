# options
message(STATUS "Building jsonio14 examples: ${BUILD_EXAMPLES}")

include_directories(${CMAKE_SOURCE_DIR})

set(Examples
        create_example
        builder_example
        parser_example
        schema_example
)

foreach (exampleName ${Examples})
    add_executable(${exampleName} ${exampleName}.cpp)
    target_link_libraries(${exampleName} jsonio14 stdc++fs )
endforeach()

add_executable(time_parser time_parser.cpp)
target_link_libraries(time_parser jsonio14 stdc++fs velocypack )

