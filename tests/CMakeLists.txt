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
  target_link_libraries(test_main gtest_main jsonio17::jsonio17-static Threads::Threads)
else()
  target_link_libraries(test_main gtest jsonio17::jsonio17 Threads::Threads)
endif()

target_include_directories(test_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src )

add_test(NAME test_main COMMAND ./tests/test_main)

#https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
#https://github.com/TusharChugh/travis-gcov-gtest-example/blob/master/.travis.yml
#ctest --extra-verbose --output-on-failure
#https://cmake.org/cmake/help/v3.7/manual/ctest.1.html
#https://cmake.org/cmake/help/v3.10/module/GoogleTest.html

