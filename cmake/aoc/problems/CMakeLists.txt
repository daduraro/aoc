set(AOC_CFG_DIR ${AOC_DIR}/cfg)

# define new function to create aoc executable for each day
function(aoc AOC_YEAR AOC_DAY DIR)
    file(GLOB sourcefiles CONFIGURE_DEPENDS "${DIR}/*")
    set(EXE_NAME aoc${AOC_YEAR}_${AOC_DAY})

    add_executable(${EXE_NAME}
        "${AOC_CFG_DIR}/main.cpp"
        ${sourcefiles}
    )
    configure_file("${AOC_CFG_DIR}/version.h.in" "${EXE_NAME}/version.h")

    set_target_properties(${EXE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED 1
    )
    target_include_directories(${EXE_NAME}
    PRIVATE
        "$<BUILD_INTERFACE:${AOC_INCLUDE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}>"
    )
    target_link_libraries(${EXE_NAME} PRIVATE aoc)
endfunction()

file(GLOB dirs LIST_DIRECTORIES true CONFIGURE_DEPENDS RELATIVE "${AOC_SOURCE_DIR}" "${AOC_SOURCE_DIR}/*/d*")
set(regex "^aoc([0-9]+)/d([0-9]+)$")
foreach(dir IN LISTS dirs)
    string(REGEX REPLACE ${regex} "\\1" year "${dir}")
    string(REGEX REPLACE ${regex} "\\2" day "${dir}")
    if (year AND day)
        aoc(${year} ${day} "${AOC_SOURCE_DIR}/${dir}")
    endif()
endforeach()


# aoc(18 01 "${AOC_INCLUDE_DIR}"
#     "${AOC_SOURCE_DIR}/aoc18/d01/main.cpp"
#     "${AOC_SOURCE_DIR}/aoc18/d01/solver.cpp"
# )
# aoc(18 02 "${AOC_INCLUDE_DIR}"
#     "${AOC_SOURCE_DIR}/aoc18/d02/main.cpp"
#     "${AOC_SOURCE_DIR}/aoc18/d02/solver.cpp"
# )
# aoc(18 03 "${AOC_INCLUDE_DIR}"
#     "${AOC_SOURCE_DIR}/aoc18/d03/main.cpp"
#     "${AOC_SOURCE_DIR}/aoc18/d03/solver.cpp"
# )
# aoc(18 04 "${AOC_INCLUDE_DIR}"
#     "${AOC_SOURCE_DIR}/aoc18/d04/main.cpp"
#     "${AOC_SOURCE_DIR}/aoc18/d04/solver.cpp"
# )