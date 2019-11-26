set(AOC_DIR ${SOURCE_DIR}/aoc)
set(AOC_SOURCE_DIR ${AOC_DIR}/src)
set(AOC_INCLUDE_DIR ${AOC_DIR}/include)
set(AOC_TEST_DIR ${AOC_DIR}/test)

set(AOC_HEADERS
    ${AOC_INCLUDE_DIR}/aoc/error.h
    ${AOC_INCLUDE_DIR}/aoc/solver.h
)

set(AOC_SOURCES
    ${AOC_SOURCE_DIR}/aoc/solver.cpp
)

add_library(aoc STATIC ${AOC_HEADERS} ${AOC_SOURCES})

set_target_properties(aoc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED 1
)

target_include_directories(aoc
PUBLIC
    $<BUILD_INTERFACE:${AOC_INCLUDE_DIR}>
)

target_link_libraries(aoc PUBLIC baselib)

if (MSVC)
    target_compile_options(aoc
    INTERFACE
        "/permissive-"
        "/Zc:rvalueCast"
        "/Zc:wchar_t"
        "/Zc:inline"
        "/Zc:forScope"
        "/Zc:externConstexpr"
        "/Zc:__cplusplus"
    )
endif()

add_subdirectory(problems)