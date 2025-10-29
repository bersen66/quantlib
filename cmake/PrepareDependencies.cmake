include(FetchContent)
include(CPM)

if (QL_BUILD_TESTS)
    # set(BUILD_GMOCK ON)
    # FetchContent_Declare(
    #   googletest
    #   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    #   DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    # )
    # # For Windows: Prevent overriding the parent project's compiler/linker settings
    # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # FetchContent_MakeAvailable(googletest)
    CPMAddPackage(
      NAME googletest
      GITHUB_REPOSITORY google/googletest
      GIT_TAG release-1.12.1
      VERSION 1.12.1
      OPTIONS "INSTALL_GTEST OFF" "BUILD_GMOCK ON" "gtest_force_shared_crt"
    )
endif()

if (QL_BUILD_BENCH)
    # set(BENCHMARK_ENABLE_TESTING NO)
    # set(__CURRENT_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    # set(CMAKE_BUILD_TYPE "Release")

    # FetchContent_Declare(
    #   googlebenchmark
    #   URL https://github.com/google/benchmark/archive/refs/tags/v1.9.3.zip
    #   DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    # )

    # FetchContent_MakeAvailable(googlebenchmark)
    # set(CMAKE_BUILD_TYPE ${__CURRENT_BUILD_TYPE})
    CPMAddPackage(
      NAME benchmark
      GITHUB_REPOSITORY google/benchmark
      VERSION 1.9.3
      OPTIONS "BENCHMARK_ENABLE_TESTING Off"
    )

endif()

