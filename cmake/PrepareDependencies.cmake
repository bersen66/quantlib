include(FetchContent)

if (QL_BUILD_TESTS)
    set(BUILD_GMOCK ON)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

endif()

if (QL_BUILD_BENCH)
    set(BENCHMARK_ENABLE_TESTING NO)

    FetchContent_Declare(
        googlebenchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.zip
    )

    FetchContent_MakeAvailable(googlebenchmark)
endif()


