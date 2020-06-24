message(STATUS "Building test suite: ${BuildTests}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pass" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fail" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

include(CTest)
enable_testing()
find_package(Threads)
add_subdirectory(googletest REQUIRED)

# -------------------------------------
# Build test program
# -------------------------------------

add_executable(test_main  main.cpp )
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
      target_link_libraries( test_main
            PUBLIC gtest
            PUBLIC jsonio14-static
            PUBLIC stdc++fs
            PUBLIC jsonarango-static
            PRIVATE velocypack
            PRIVATE libcurl
            PUBLIC pthread )
else()
      target_link_libraries( test_main
           PUBLIC gtest
           PUBLIC jsonio14
           PUBLIC stdc++fs
           PUBLIC jsonarango
           PUBLIC velocypack
           PUBLIC curl
           PUBLIC pthread )
endif()
#target_link_libraries(test_main gtest jsonio14 stdc++fs  jsonarango curl  velocypack pthread ${CMAKE_THREAD_LIBS_INIT} )

target_include_directories(test_main PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_test(NAME test_main COMMAND test_main)
