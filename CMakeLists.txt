cmake_minimum_required(VERSION 3.20)
project(quantlib LANGUAGES CXX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND QL_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(QlHelpers)

# If project included using `add_subdirectory(quantlib)`
# we don't need to perform proper installation
if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    option(QUANTLIB_ENABLE_INSTALL "Enable installing" OFF)
else()
    option(QUANTLIB_ENABLE_INSTALL "Enable installing" ON)
endif()


add_subdirectory(ql)

# TODO: INSTALLING
if (QUANTLIB_ENABLE_INSTALL)

endif()
