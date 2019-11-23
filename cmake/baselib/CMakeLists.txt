set(BASELIB_DIR ${SOURCE_DIR}/baselib)
set(BASELIB_SOURCE_DIR ${BASELIB_DIR}/src)
set(BASELIB_INCLUDE_DIR ${BASELIB_DIR}/include)
set(BASELIB_TEST_DIR ${BASELIB_DIR}/test)

set(BASELIB_HEADERS
    ${BASELIB_INCLUDE_DIR}/ddr/utils/meta.h
    ${BASELIB_INCLUDE_DIR}/ddr/utils/optional_ref.h
)

set(BASELIB_SOURCES
)

add_library(baselib INTERFACE)

# set_target_properties(baselib PROPERTIES
#     CXX_STANDARD 17
#     CXX_STANDARD_REQUIRED 1
# )

target_include_directories(baselib
INTERFACE
    $<BUILD_INTERFACE:${BASELIB_INCLUDE_DIR}>
)

if (MSVC)
    target_compile_options(baselib
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