ql_cpp_library(
    NAME dummy
    HDRS
      "dummy.h"
    SRCS
      "dummy.cc"
)

ql_cpp_test (
    NAME dummy_test
    SRCS
      "dummy_test.cc"
    DEPS
      ql::dummy
      GTest::gtest_main
)

ql_cpp_executable(
    NAME dummy_benchmark
    SRCS "dummy_benchmark.cc"
    DEPS
      ql::dummy
      benchmark::benchmark
    BENCH
)

