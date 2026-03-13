cmake_minimum_required(VERSION 3.12...3.27.4)
project(antibot LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
  include(CheckLinkerFlag)
endif()

set(ORIGINAL_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(ORIGINAL_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
set(ORIGINAL_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
set(OWN_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${OWN_CMAKE_MODULE_PATH})

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(TARGET_BITS "32")
else()
  set(TARGET_BITS "64")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(TARGET_OS "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(TARGET_OS "linux")
endif()

########################################################################
# Common functions
########################################################################

macro(set_glob VAR DIRECTORY)
  set(${VAR} ${ARGN})
  if(${VAR})
    list(TRANSFORM ${VAR} PREPEND "${PROJECT_SOURCE_DIR}/${DIRECTORY}/")
  endif()
endmacro()

macro(set_src VAR DIRECTORY)
  set_glob(${VAR} ${DIRECTORY} ${ARGN})
endmacro()

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
# Dependencies
########################################################################

set(LIB_DIR "${TARGET_OS}/lib${TARGET_BITS}")

function(set_extra_dirs_lib VARIABLE NAME)
  set("PATHS_${VARIABLE}_LIBDIR" PARENT_SCOPE)
  set("HINTS_${VARIABLE}_LIBDIR" PARENT_SCOPE)
  if(PREFER_BUNDLED_LIBS)
    set(TYPE HINTS)
  else()
    set(TYPE PATHS)
  endif()
  if(TARGET_BITS AND TARGET_OS)
    set(DIR "ddnet-libs/${NAME}/${LIB_DIR}")
    set("${TYPE}_${VARIABLE}_LIBDIR" "${DIR}" PARENT_SCOPE)
    set("EXTRA_${VARIABLE}_LIBDIR" "${DIR}" PARENT_SCOPE)
  endif()
endfunction()

function(set_extra_dirs_include VARIABLE NAME LIBRARY)
  set("PATHS_${VARIABLE}_INCLUDEDIR" PARENT_SCOPE)
  set("HINTS_${VARIABLE}_INCLUDEDIR" PARENT_SCOPE)
  is_bundled(IS_BUNDLED "${LIBRARY}")
  if(IS_BUNDLED)
    set(TMP_TARGET_OS ${TARGET_OS})
    if(TARGET_OS STREQUAL "emscripten")
      set(TMP_TARGET_OS webasm)
    endif()
    set("HINTS_${VARIABLE}_INCLUDEDIR" "ddnet-libs/${NAME}/include" "ddnet-libs/${NAME}/include/${TMP_TARGET_OS}" PARENT_SCOPE)
  endif()
endfunction()

function(is_bundled VARIABLE PATH)
  if(PATH)
    string(FIND "${PATH}" "${PROJECT_SOURCE_DIR}" LOCAL_PATH_POS)
    if(LOCAL_PATH_POS EQUAL 0 AND TARGET_BITS AND TARGET_OS)
      set("${VARIABLE}" ON PARENT_SCOPE)
    else()
      set("${VARIABLE}" OFF PARENT_SCOPE)
    endif()
  else()
    set("${VARIABLE}" OFF PARENT_SCOPE)
  endif()
endfunction()

find_package(ZLIB)
find_package(Curl)

if(NOT(CURL_FOUND))
  message(SEND_ERROR "You must install Curl to compile ${CMAKE_PROJECT_NAME}")
endif()

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
list(FILTER POLYBOB_SRCS EXCLUDE REGEX "engine/external/json-parser")
list(FILTER POLYBOB_SRCS EXCLUDE REGEX "engine/external/md5")
list(FILTER POLYBOB_SRCS EXCLUDE REGEX "engine/external/zlib")

file(GLOB_RECURSE ANTIBOB_SRCS
    src/antibob/interface.cpp
    src/antibob/*/*.cpp
)

# External dependencies
set(TARGETS_DEP)

set_src(DEP_JSON_SRC src/ddnet/polybob/engine/external/json-parser json.cpp json.h)
add_library(json EXCLUDE_FROM_ALL OBJECT ${DEP_JSON_SRC})
set(DEP_JSON $<TARGET_OBJECTS:json>)

set_src(DEP_MD5_SRC src/ddnet/polybob/engine/external/md5 md5.cpp md5.h)
add_library(md5 EXCLUDE_FROM_ALL OBJECT ${DEP_MD5_SRC})
set(DEP_MD5 $<TARGET_OBJECTS:md5>)

list(APPEND TARGETS_DEP json md5)

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

set(DEPENDENCIES ${DEP_JSON} ${DEP_MD5} ${ZLIB_DEP} ${CMAKE_BINARY_DIR}/generated/git_revision.cpp)

# Targets
set(TARGETS_OWN)

set(PLATFORM_LIBS)
if(TARGET_OS STREQUAL "windows")
    list(APPEND PLATFORM_LIBS shlwapi.lib) # PathIsRelativeW
    list(APPEND PLATFORM_LIBS version.lib) # GetFileVersionInfoSizeW
    list(APPEND PLATFORM_LIBS ws2_32.lib) # Windows sockets
    list(APPEND PLATFORM_LIBS advapi32.lib) # Reg* and Crypt* functions
    list(APPEND PLATFORM_LIBS ole32.lib) # CoInitialize(Ex)
    list(APPEND PLATFORM_LIBS shell32.lib)
    list(APPEND PLATFORM_LIBS ntdll.lib) # https://github.com/ddnet/ddnet/issues/6725
    list(APPEND PLATFORM_LIBS uuid.lib) # FOLDERID_RoamingAppData
endif()

set(LIBS
    ${CURL_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${PLATFORM_LIBS}
)

# Static library for the tests
add_library(antibot_static STATIC
        ${POLYBOB_SRCS}
        ${ANTIBOB_SRCS}
        ${DEPENDENCIES}
)
list(APPEND TARGETS_OWN antibot_static)
target_link_libraries(antibot_static PUBLIC ${LIBS})

# Shared library for use
add_library(antibot SHARED
        ${POLYBOB_SRCS}
        ${ANTIBOB_SRCS}
        ${DEPENDENCIES}
)
list(APPEND TARGETS_OWN antibot)
target_link_libraries(antibot PRIVATE ${LIBS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(COPY_FILES
        ${CURL_COPY_FILES}
)
file(COPY ${COPY_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

########################################################################
# Apply options to targets
########################################################################

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

set(TARGETS ${TARGETS_OWN} ${TARGETS_DEP})

foreach(target ${TARGETS})
  if(MSVC)
    target_compile_options(${target} PRIVATE /Zc:__cplusplus) # Fixes the __cplusplus macro
    target_compile_options(${target} PRIVATE /MP) # Use multiple cores
    target_compile_options(${target} PRIVATE /EHsc) # Only catch C++ exceptions with catch.
    target_compile_options(${target} PRIVATE /GS) # Protect the stack pointer.
    target_compile_options(${target} PRIVATE /wd4996) # Use of non-_s functions.
    target_compile_options(${target} PRIVATE /utf-8) # Use UTF-8 for source files.
  endif()
endforeach()

foreach(target ${TARGETS_OWN})
  if(MSVC)
    target_compile_options(${target} PRIVATE /wd4244) # Possible loss of data (float -> int, int -> float, etc.).
    target_compile_options(${target} PRIVATE /wd4267) # Possible loss of data (size_t - int on win64).
    target_compile_options(${target} PRIVATE /wd4800) # Implicit conversion of int to bool.
    target_compile_options(${target} PRIVATE /wd5055) # C++20 operator deprecated between enumerations and floating-point types
  endif()
  if(TARGET_OS STREQUAL "windows")
    # See https://learn.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers
    target_compile_definitions(${target} PRIVATE NOMINMAX) # windows.h shouldn't define min/max macros
    target_compile_definitions(${target} PRIVATE WIN32_LEAN_AND_MEAN) # windows.h shouldn't define the name IStorage
    target_compile_definitions(${target} PRIVATE NOGDI) # wingdi.h shouldn't define the name ERROR
    target_compile_definitions(${target} PRIVATE NTDDI_VERSION=0x06000000) # Minimum OS version (new macro, since Vista)
    target_compile_definitions(${target} PRIVATE _WIN32_WINNT=0x0600) # Minimum OS version (old macro, both must be defined)
    target_compile_definitions(${target} PRIVATE UNICODE) # Windows headers
    target_compile_definitions(${target} PRIVATE _UNICODE) # C-runtime
  endif()
  target_include_directories(${target} SYSTEM PRIVATE ${CURL_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
  if(OUR_FLAGS_OWN)
    target_compile_options(${target} PRIVATE ${OUR_FLAGS_OWN})
  endif()
endforeach()

foreach(target ${TARGETS_DEP})
  if(MSVC)
    target_compile_options(${target} PRIVATE /W0)
  endif()
endforeach()

########################################################################
# Tests
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
    target_link_libraries(${TEST_NAME}_bob_test PRIVATE antibot_static)
    target_compile_options(${TEST_NAME}_bob_test PRIVATE ${OUR_FLAGS_OWN})
    target_include_directories(${TEST_NAME}_bob_test PRIVATE src/test)
endforeach()

# tester runner target "make test"
enable_testing()
file(GLOB TEST_BINARIES
    ${CMAKE_BINARY_DIR}/bin/*_bob_test
)
foreach(TEST_BIN ${TEST_BINARIES})
    get_filename_component(TEST_BIN_NAME ${TEST_BIN} NAME_WE)
    add_test(NAME ${TEST_BIN_NAME} COMMAND ${TEST_BIN})
endforeach()

