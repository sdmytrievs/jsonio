# options
message(STATUS "Building jsonio examples: ${BuildExamples}")

include_directories(${CMAKE_SOURCE_DIR})
find_package(Threads REQUIRED)

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
     target_link_libraries(${exampleName} jsonio::jsonio-static Threads::Threads )
  else()
     target_link_libraries(${exampleName} jsonio::jsonio Threads::Threads)
  endif()
endforeach()

