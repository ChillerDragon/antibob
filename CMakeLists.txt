cmake_minimum_required(VERSION 3.12...3.27.4)
project(antibot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
  include(CheckLinkerFlag)
endif()

########################################################################
# Compiler flags
########################################################################

function(add_c_compiler_flag_if_supported VARIABLE FLAG)
  if(ARGC GREATER 2)
    set(CHECKED_FLAG "${ARGV2}")
  else()
    set(CHECKED_FLAG "${FLAG}")
  endif()
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" CONFIG_VARIABLE "FLAG_SUPPORTED${CHECKED_FLAG}")
  check_c_compiler_flag("${CHECKED_FLAG}" ${CONFIG_VARIABLE})
  if(${CONFIG_VARIABLE})
    if(${VARIABLE})
      set("${VARIABLE}" "${${VARIABLE}};${FLAG}" PARENT_SCOPE)
    else()
      set("${VARIABLE}" "${FLAG}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(add_cxx_compiler_flag_if_supported VARIABLE FLAG)
  if(ARGC GREATER 2)
    set(CHECKED_FLAG "${ARGV2}")
  else()
    set(CHECKED_FLAG "${FLAG}")
  endif()
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" CONFIG_VARIABLE "FLAG_SUPPORTED${CHECKED_FLAG}")
  check_cxx_compiler_flag("${CHECKED_FLAG}" ${CONFIG_VARIABLE})
  if(${CONFIG_VARIABLE})
    if(${VARIABLE})
      set("${VARIABLE}" "${${VARIABLE}};${FLAG}" PARENT_SCOPE)
    else()
      set("${VARIABLE}" "${FLAG}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(add_linker_flag_if_supported VARIABLE FLAG)
  if(ARGC GREATER 2)
    set(CHECKED_FLAG "${ARGV2}")
  else()
    set(CHECKED_FLAG "${FLAG}")
  endif()
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" CONFIG_VARIABLE "FLAG_SUPPORTED${CHECKED_FLAG}")
  if(CMAKE_VERSION VERSION_LESS 3.18)
    set(${CONFIG_VARIABLE} OFF)
  else()
    check_linker_flag(C "${CHECKED_FLAG}" ${CONFIG_VARIABLE})
  endif()
  if(${CONFIG_VARIABLE})
    if(${VARIABLE})
      set("${VARIABLE}" "${${VARIABLE}};${FLAG}" PARENT_SCOPE)
    else()
      set("${VARIABLE}" "${FLAG}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

########################################################################
# antibob
########################################################################

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

# antibob specific, not sure what we need it for
add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -fPIC)

# TODO: enable these
# add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wall)
# add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wextra)

add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wshadow-all) # clang
add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wshadow=global) # gcc
add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wthread-safety)
add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wthread-safety-negative)
add_cxx_compiler_flag_if_supported(OUR_FLAGS_OWN -Wno-deprecated-enum-float-conversion) # c++20 arithmetic between floating-point type and enumeration type

if(OUR_FLAGS_OWN)
    target_compile_options(antibot PRIVATE ${OUR_FLAGS_OWN})
endif()

########################################################################
# tests
########################################################################

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
    target_compile_options(${TEST_NAME}_bob_test PRIVATE ${OUR_FLAGS_OWN})
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

