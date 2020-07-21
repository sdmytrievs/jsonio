# options
message(STATUS "Building jsonio17 examples: ${BUILD_EXAMPLES}")

include_directories(${CMAKE_SOURCE_DIR})

set(Examples
        create_example
        builder_example
        parser_example
        schema_example
        coll_time_test
        coll_query_example
        time_parser )

foreach (exampleName ${Examples})
    add_executable(${exampleName} ${exampleName}.cpp)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
          target_link_libraries(${exampleName}
                PUBLIC jsonio17-static
                PUBLIC stdc++fs
                PUBLIC jsonarango-static
                PRIVATE velocypack
                PRIVATE libcurl
                PUBLIC pthread )
   elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")

          target_link_libraries( ${exampleName}
                PRIVATE jsonio17
                PUBLIC jsonarango
                PUBLIC velocypack
                PUBLIC curl
                PUBLIC pthread )
    else()
          target_link_libraries( ${exampleName}
               PRIVATE jsonio17
               PUBLIC stdc++fs
               PUBLIC jsonarango
               PUBLIC velocypack
               PUBLIC curl
               PUBLIC pthread )
    endif()
#target_link_libraries(${exampleName} jsonio17 stdc++fs  jsonarango curl  velocypack pthread )
endforeach()

#add_executable(time_parser time_parser.cpp)
#target_link_libraries(time_parser jsonio17 stdc++fs jsonarango curl  velocypack )

