# ==============================================================================
# QL CMake Build Helpers
#
# This file defines a set of CMake functions that provide a lightweight
# abstraction layer similar to Bazel build rules.
#
# These functions follow a consistent naming convention and automatically
# prefix all generated targets with `ql_`, while also providing convenient
# alias targets of the form `ql::<name>`.
#
# Defined functions:
#   - ql_cpp_library()     — define a C++ library (shared or header-only)
#   - ql_cpp_test()        — define a unit test target (integrated with CTest)
#   - ql_cpp_executable()  — define a standalone or benchmark executable
#
# All functions respect the following build-time options:
#   QL_BUILD_TESTS   — enable or disable test targets
#   QL_BUILD_BENCH   — enable or disable benchmark targets
#
# Dependencies:
#   include(GNUInstallDirs)
#   include(CMakeParseArguments)
#
# Example:
#   ql_cpp_library(
#     NAME math
#     SRCS "math_utils.cc"
#     HDRS "math_utils.h"
#     DEPS ql::core
#   )
#
#   ql_cpp_test(
#     NAME math_test
#     SRCS "math_test.cc"
#     DEPS ql::math GTest::gtest_main
#   )
#
#   ql_cpp_executable(
#     NAME benchmark_runner
#     SRCS "bench_main.cc"
#     DEPS ql::core benchmark::benchmark
#     BENCH
#   )
# ==============================================================================
include(GNUInstallDirs)
include(CMakeParseArguments)

# ql_cpp_library()
#
# CMake function to create a C++ library in the QL project style.
#
# Parameters:
#   NAME:        Name of the library (required)
#   HDRS:        List of header files (optional)
#   SRCS:        List of source files (.cc, .cpp, etc.)
#   DEPS:        List of dependent libraries to link against
#   COPTS:       List of private compile options
#
# Options:
#   PUBLIC:      Marks the library as publicly visible (for installation)
#   TESTONLY:    Builds the library only when QL_BUILD_TESTS is enabled
#   BENCHONLY:   Builds the library only when QL_BUILD_BENCH is enabled
#
# Behavior:
#   - Automatically prefixes target names with 'ql_' (e.g., ql_math).
#   - Creates alias target `ql::<name>` for convenient linking.
#   - If no source files are provided, creates a header-only INTERFACE library.
#   - Includes default QL include directories for both build and install interfaces.
#   - Automatically filters out header files from SRCS if accidentally listed.
#
# Note:
#   Header-only libraries are created as INTERFACE targets,
#   while others are built as SHARED libraries.
#
# Usage:
#   ql_cpp_library(
#     NAME
#       math
#     HDRS
#       "math_utils.h"
#     SRCS
#       "math_utils.cc"
#     DEPS
#       ql::core
#     COPTS
#       "-Wall" "-Wextra"
#     PUBLIC
#   )
function(ql_cpp_library)
    set(options PUBLIC TESTONLY BENCHONLY)
    set(oneValueArgs NAME)
    set(multiValueArgs HDRS SRCS DEPS COPTS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_NAME "ql_${ARGS_NAME}")

    if (ARGS_NAME STREQUAL "")
        message(FATAL_ERROR "Name of library is required")
    endif()

    if (NOT ${QL_BUILD_TESTS} AND ${ARGS_TESTONLY})
        return()
    endif()

    if (NOT ${QL_BUILD_BENCH} AND ${ARGS_BENCHONLY})
        return()
    endif()

    set(QL_SOURCES "${ARGS_SRCS}")
    foreach(src_file IN LISTS QL_SOURCES)
        if(${src_file} MATCHES ".*\\.(h|inc|hpp|hh)")
            list(REMOVE_ITEM QL_SOURCES "${src_file}")
        endif()
    endforeach()

    if(QL_SOURCES STREQUAL "")
        set(QL_LIB_IS_HEADER_ONLY 1)
    else()
        set(QL_LIB_IS_HEADER_ONLY 0)
    endif()

    if(QL_LIB_IS_HEADER_ONLY)
        add_library(${_NAME} INTERFACE)
        target_include_directories(${_NAME}
            INTERFACE
            "$<BUILD_INTERFACE:${QL_COMMON_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
        target_link_libraries(${_NAME} INTERFACE ${ARGS_DEPS})
    else()
        if(ARGS_PUBLIC)
            add_library(${_NAME} SHARED ${QL_SOURCES})
            if(WIN32)
                # Экспорт всех символов для публичной DLL
                set_target_properties(${_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
            endif()
            target_include_directories(${_NAME}
                PUBLIC
                "$<BUILD_INTERFACE:${QL_COMMON_INCLUDE_DIRS}>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
            )
            target_link_libraries(${_NAME} PUBLIC ${ARGS_DEPS})
        else()
            # Для непубличных делаем static
            add_library(${_NAME} STATIC ${QL_SOURCES})
            target_include_directories(${_NAME}
                PUBLIC
                "$<BUILD_INTERFACE:${QL_COMMON_INCLUDE_DIRS}>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
            )
            target_link_libraries(${_NAME} PUBLIC ${ARGS_DEPS})
        endif()

        target_compile_options(${_NAME} PRIVATE ${ARGS_COPTS})
    endif()

    add_library(ql::${ARGS_NAME} ALIAS ${_NAME})
endfunction()

# ql_cpp_test()
#
# CMake function to define a unit test target in the QL project style.
# This imitates Bazel's cc_test rule.
#
# Parameters:
#   NAME:   Name of the test target (required)
#   SRCS:   List of source files for the test binary
#   DEPS:   List of dependent libraries to link with the test
#   COPTS:  List of private compile options
#
# Behavior:
#   - Creates an executable target with the specified sources.
#   - Links it against the listed dependencies.
#   - Registers it as a CTest target with `add_test`.
#   - Skips creation if QL_BUILD_TESTS is disabled.
#
# Usage:
#   ql_cpp_test(
#     NAME
#       math_test
#     SRCS
#       "math_test.cc"
#     DEPS
#       ql::math
#       GTest::gmock
#       GTest::gtest_main
#     COPTS
#       "-O0" "-g"
#   )
function(ql_cpp_test)
    if (NOT ${QL_BUILD_TESTS})
        return()
    endif()

    cmake_parse_arguments(
        ARGS
        ""
        "NAME"
        "SRCS;COPTS;DEPS"
        ${ARGN}
    )

    set(_NAME ${ARGS_NAME})
    if (${_NAME} STREQUAL "")
        message(FATAL_ERROR "Test must have name")
    endif()

    add_executable(${_NAME} ${ARGS_SRCS})
    target_compile_options(${_NAME} PRIVATE ${ARGS_COPTS})
    target_link_libraries(${_NAME} PUBLIC ${ARGS_DEPS})
    add_test(NAME ${_NAME} COMMAND ${_NAME})
endfunction()

# ql_cpp_executable()
#
# CMake function to define an executable target in the QL project style.
#
# Parameters:
#   NAME:   Name of the executable (required)
#   SRCS:   List of source files
#   HDRS:   List of header files (optional)
#   DEPS:   List of dependent libraries to link against
#   COPTS:  List of private compile options
#
# Options:
#   BENCH:  If specified, the target is built only when QL_BUILD_BENCH is enabled.
#
# Behavior:
#   - Creates an executable with the given sources.
#   - Applies compile options and links dependencies.
#   - Skips creation if BENCH is specified and QL_BUILD_BENCH is disabled.
#
# Note:
#   Future versions may include installation rules for executables.
#
# Usage:
#   ql_cpp_executable(
#     NAME
#       benchmark_runner
#     SRCS
#       "bench_main.cc"
#     DEPS
#       ql::core
#       benchmark::benchmark
#     COPTS
#       "-O3"
#     BENCH
#   )
function(ql_cpp_executable)

    cmake_parse_arguments(
        ARGS
        "BENCH"
        "NAME"
        "SRCS;COPTS;HDRS;DEPS"
        ${ARGN}
    )

    set(_NAME ${ARGS_NAME})

    if (${_NAME} STREQUAL "")
        message(FATAL_ERROR "Name for executable must be provided")
    endif()

    if (NOT ${QL_BUILD_BENCH} AND ${ARGS_BENCH})
        return()
    endif()

    add_executable(${_NAME} ${ARGS_SRCS})
    target_compile_options(${_NAME} PRIVATE ${ARGS_COPTS})
    target_link_libraries(${_NAME} PUBLIC ${ARGS_DEPS})

    # TODO: Installation

endfunction()
