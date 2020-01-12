# Advent of Code — C++17 solutions

This repository are some of the solutions for the Advent of Code challenge (https://adventofcode.com/). My goal is that every solution should accept any input (and gracefully fail if it is not valid), be correct (always finish to execute and give a/the correct answer) and still be reasonably fast.

## How to build

First, update the input submodule (https://github.com/daduraro/aoc-data) if not done already:

```bash
git submodule init      # populates fetch/push url from .gitmodules
git submodule update    # pulls the submodules
```

Then, build using CMake (>=3.13):

```bash
mkdir build && cd build
cmake ..
# build using ninja / msvc / make / ...
```

This will generate an executable for each year and day of the Advent of Code. Every generated executable have the same CLI in which the directory "input" should be passed as first argument. For example:

```bash
$ pwd
.../aoc/build
$ ./.../aoc2018_05 ../input
"input.in"          parse:    (3371 us)    partA:   [PASSED]  (27376 us)    partB:   [PASSED] (188508 us)
"sample-1.in"       parse:      (11 us)    partA:   [PASSED]     (10 us)    partB:   [PASSED]     (56 us)
```

## Directory structure

```
cmake/                      - Where the CMakeLists are located
source/                     - The source files of the project
source/aoc/cfg              - Configure files used by CMake to generate
                              main.cpp and version.h for each executable
source/aoc/include          - Headers common across aoc executables
source/aoc/src/aoc          - Files common across aoc executables
source/aoc/src/aocYYYY/dXX  - Files for problem of year YYYY and day XX
source/baselib              - Library with utilities that are not particularly
                              tied with aoc (math, data structures...)
```

## Problem status

### Legend

| Symbol | Meaning |
|-------:|---------|
| 🟢    | Solution works and is complete. |
| 🔵    | Solution works and is complete but there is some TODO (solution is inefficient, some input may not be validated, ...). |
| 🟡    | Partial solution (partB is missing, some valid input fail, ...) |
| 🔴    | No solution yet. |

### 2015-2017

TBD

### 2018

| Day | Status | Day | Status | Day | Status |
|----:|:------:|----:|:------:|----:|:------:|
| 01  | 🔵    | 10  | 🔴     | 19  | 🔴    |
| 02  | 🟢    | 11  | 🔴     | 20  | 🔴    |
| 03  | 🟢    | 12  | 🔴     | 21  | 🔴    |
| 04  | 🟢    | 13  | 🔴     | 22  | 🔴    |
| 05  | 🟢    | 14  | 🔴     | 23  | 🔴    |
| 06  | 🟢    | 15  | 🔴     | 24  | 🔴    |
| 07  | 🟢    | 16  | 🔴     | 25  | 🔴    |
| 08  | 🟢    | 17  | 🔴     |
| 09  | 🔴    | 18  | 🔴     |

### 2019

| Day | Status | Day | Status | Day | Status |
|----:|:------:|----:|:------:|----:|:------:|
| 01  | 🟢    | 10  | 🔴     | 19  | 🔴    |
| 02  | 🟢    | 11  | 🔴     | 20  | 🔴    |
| 03  | 🔵    | 12  | 🔴     | 21  | 🔴    |
| 04  | 🔵    | 13  | 🔴     | 22  | 🔴    |
| 05  | 🔴    | 14  | 🔴     | 23  | 🔴    |
| 06  | 🔴    | 15  | 🔴     | 24  | 🔴    |
| 07  | 🔴    | 16  | 🔴     | 25  | 🔴    |
| 08  | 🔴    | 17  | 🔴     |
| 09  | 🔴    | 18  | 🔴     |