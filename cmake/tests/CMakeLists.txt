add_subdirectory("${EXTERN_DIR}/googletest" "extern/googletest")

# cleaner cache variables for GUI
mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

# cleaner visibility of the projects
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

set(TEST_DIR "${SOURCE_DIR}/unittest")
set(TEST_SOURCE_DIR "${TEST_DIR}/src")
set(TEST_INCLUDE_DIR "${TEST_DIR}/include")

# add test runner
list(APPEND TEST_SOURCES "${TEST_SOURCE_DIR}/main.cpp")

add_executable(unittest ${TEST_SOURCES})

set_target_properties(unittest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED 1
)

target_include_directories(unittest
PUBLIC
    $<BUILD_INTERFACE:${TEST_INCLUDE_DIR}>
)

target_link_libraries(unittest PRIVATE aoc gtest gmock gtest_main)

include(GoogleTest)
gtest_discover_tests(unittest)