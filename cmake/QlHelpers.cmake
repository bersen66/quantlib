include(GNUInstallDirs)
include(CMakeParseArguments)

# * HDRS   -- library public headers
# * SRCS   -- library sources
# * COPTS  -- library privte compiler options
# * DEPS   -- library deps
# * PUBLIC -- library will be exported
function(ql_cpp_library)
    set(options PUBLIC)
    set(oneValueArgs NAME)
    set(multiValueArgs HDRS SRCS DEPS COPTS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_NAME "ql_${ARGS_NAME}")

    if (ARGS_NAME STREQUAL "")
        message(FATAL_ERROR "Name of library is required")
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


    if (QL_LIB_IS_HEADER_ONLY)
        add_library(${_NAME} INTERFACE)
        target_include_directories(${_NAME}
            INTERFACE
            "$<BUILD_INTERFACE:${QL_COMMON_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
        target_link_libraries(${_NAME} INTERFACE ${ARGS_DEPS})
    else()
        add_library(${_NAME} SHARED ${ARGS_SRCS})
        target_include_directories(${_NAME}
            PUBLIC
            "$<BUILD_INTERFACE:${QL_COMMON_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

        target_link_libraries(${_NAME} PUBLIC ${ARGS_DEPS})
        target_compile_options(${_NAME} PRIVATE ${ARGS_COPTS})
    endif()

    add_library(ql::${ARGS_NAME} ALIAS ${_NAME})

endfunction()

# absl_cc_test()
#
# CMake function to imitate Bazel's cc_test rule.
#
# Parameters:
# NAME: name of target (see Usage below)
# SRCS: List of source files for the binary
# DEPS: List of other libraries to be linked in to the binary targets
# COPTS: List of private compile options
# DEFINES: List of public defines
# LINKOPTS: List of link options
#
# Note:
# By default, absl_cc_test will always create a binary named absl_${NAME}.
# This will also add it to ctest list as absl_${NAME}.
#
# Usage:
# absl_cc_library(
#   NAME
#     awesome
#   HDRS
#     "a.h"
#   SRCS
#     "a.cc"
#   PUBLIC
# )
#
# absl_cc_test(
#   NAME
#     awesome_test
#   SRCS
#     "awesome_test.cc"
#   DEPS
#     absl::awesome
#     GTest::gmock
#     GTest::gtest_main
# )
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
