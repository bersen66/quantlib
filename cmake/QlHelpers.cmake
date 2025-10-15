include(GNUInstallDirs)
include(CMakeParseArguments)

# * HDRS   -- library public headers
# * SRCS   -- library sources
# * COPTS  -- library privte compiler options
# * DEPS   -- library deps
# * PUBLIC -- library will be exported
function(ql_cc_library)
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
