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
    src/ddnet/polybob/game/server/*.cpp
    src/ddnet/polybob/engine/shared/*.cpp
)

file(GLOB_RECURSE ANTIBOB_SRCS
    src/antibob/interface.cpp
    src/antibob/*/*.cpp
)

# External dependencies
file(GLOB_RECURSE ZLIB_DEP src/ddnet/polybob/engine/external/zlib/*.cpp)
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

set(DEPENDENCIES ${DEPENDENCIES} ${ZLIB_DEP} ${CMAKE_BINARY_DIR}/generated/git_revision.cpp)

add_library(antibot SHARED
    ${POLYBOB_SRCS}
    ${ANTIBOB_SRCS}
    ${DEPENDENCIES}
)

set(PLATFORM_LIBS)
list(APPEND PLATFORM_LIBS shlwapi.lib) # PathIsRelativeW
list(APPEND PLATFORM_LIBS version.lib) # GetFileVersionInfoSizeW
list(APPEND PLATFORM_LIBS ws2_32.lib) # Windows sockets
list(APPEND PLATFORM_LIBS advapi32.lib) # Reg* and Crypt* functions
list(APPEND PLATFORM_LIBS ole32.lib) # CoInitialize(Ex)
list(APPEND PLATFORM_LIBS shell32.lib)
list(APPEND PLATFORM_LIBS ntdll.lib) # https://github.com/ddnet/ddnet/issues/6725
list(APPEND PLATFORM_LIBS uuid.lib) # FOLDERID_RoamingAppData

target_link_libraries(antibot ${PLATFORM_LIBS})

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

  target_compile_options(antibot PRIVATE /wd4244) # Possible loss of data (float -> int, int -> float, etc.).
  target_compile_options(antibot PRIVATE /wd4267) # Possible loss of data (size_t - int on win64).
  target_compile_options(antibot PRIVATE /wd4800) # Implicit conversion of int to bool.
  target_compile_options(antibot PRIVATE /wd5055) # C++20 operator deprecated between enumerations and floating-point types
  target_compile_options(antibot PRIVATE /Zc:__cplusplus) # Fixes the __cplusplus macro
  target_compile_options(antibot PRIVATE /MP) # Use multiple cores
  target_compile_options(antibot PRIVATE /EHsc) # Only catch C++ exceptions with catch.
  target_compile_options(antibot PRIVATE /GS) # Protect the stack pointer.
  target_compile_options(antibot PRIVATE /wd4996) # Use of non-_s functions.
  target_compile_options(antibot PRIVATE /utf-8) # Use UTF-8 for source files.
  target_compile_definitions(antibot PRIVATE NOMINMAX) # windows.h shouldn't define min/max macros
  target_compile_definitions(antibot PRIVATE WIN32_LEAN_AND_MEAN) # windows.h shouldn't define the name IStorage
  target_compile_definitions(antibot PRIVATE NOGDI) # wingdi.h shouldn't define the name ERROR
  # 0x0501 (Windows XP) is required for mingw to get getaddrinfo to work
  # 0x0600 (Windows Vista) is required to use RegGetValueW and RegDeleteTreeW
  target_compile_definitions(antibot PRIVATE NTDDI_VERSION=0x06000000) # Minimum OS version (new macro, since Vista)
  target_compile_definitions(antibot PRIVATE _WIN32_WINNT=0x0600) # Minimum OS version (old macro, both must be defined)
  target_compile_definitions(antibot PRIVATE UNICODE) # Windows headers
  target_compile_definitions(antibot PRIVATE _UNICODE) # C-runtime

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

