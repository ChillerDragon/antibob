cmake_minimum_required(VERSION 3.10)
project(antibot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Og -g -MMD -MP")

# Include directories
include_directories(
    src/antibob
    src/ddnet
)

# Find all source files
file(GLOB_RECURSE POLYBOB_SRCS
    src/ddnet/polybob/*/*.cpp
    src/ddnet/polybob/base/unicode/*.cpp
    src/ddnet/polybob/base/system/*.cpp
    src/ddnet/polybob/game/generated/*.cpp
    src/ddnet/polybob/engine/shared/*.cpp
)

file(GLOB_RECURSE ANTIBOB_SRCS
    src/antibob/interface.cpp
    src/antibob/*/*.cpp
)

# External dependencies
set(DEPENDENCIES
    src/ddnet/polybob/engine/external/md5/md5.cpp
    src/ddnet/polybob/engine/external/json-parser/json.cpp
)

# Generate git revision
execute_process(
    COMMAND git rev-parse --short=16 HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Create git revision file
file(WRITE ${CMAKE_BINARY_DIR}/generated/git_revision.cpp
    "const char *BOB_GIT_SHORTREV_HASH = \"${GIT_HASH}\";"
)

set(DEPENDENCIES ${DEPENDENCIES} ${CMAKE_BINARY_DIR}/generated/git_revision.cpp)

add_library(antibot SHARED
    ${POLYBOB_SRCS}
    ${ANTIBOB_SRCS}
    ${DEPENDENCIES}
)

target_link_libraries(antibot z)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# compile test binaries
file(GLOB BOBTEST_FRAMEWORK_SRCS
    src/test/*.cpp
)

file(GLOB_RECURSE TESTS_SRCS
    src/test/bob/*.cpp
)
foreach(TEST_SRC ${TESTS_SRCS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME}_bob_test ${TEST_SRC} ${BOBTEST_FRAMEWORK_SRCS})
    target_link_libraries(${TEST_NAME}_bob_test antibot z curl)
    include_directories(src/test)
endforeach()

# tester runner target "make test"
enable_testing()
file(GLOB TEST_BINARIES
    ${CMAKE_BINARY_DIR}/bin/*_bob_test
)
foreach(TEST_BIN ${TEST_BINARIES})
    add_test(NAME ${TEST_BIN} COMMAND ${TEST_BIN})
endforeach()

