# configure project
project(aoc LANGUAGES CXX VERSION 0.1)

# configure directories
get_filename_component(ROOT_DIR ".." ABSOLUTE)
set(SOURCE_DIR ${ROOT_DIR}/source)
set(EXTERN_DIR ${ROOT_DIR}/extern)

# macro to add gtest
#set(TEST_SOURCES "")
#macro(package_add_test TESTNAME)
#  set(TEST_SOURCES ${TEST_SOURCES} ${ARGN}) # this is just so that reading the variable works
#  set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE) # this is the actual TEST_SOURCES, only if just 1 scope deeper (function/directories add scope)
#endmacro()

add_subdirectory(baselib)
add_subdirectory(aoc)

#option(AOC_TESTS "Build the tests" OFF)
#if (AOC_TESTS)
#    enable_testing()
#    add_subdirectory(tests)
#endif()