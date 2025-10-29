ql_cpp_library(
    NAME swaps
    HDRS "swaps.h"
    SRCS "swaps.cc"
    DEPS 
      ql::types
      ql::generator
    PUBLIC
)

ql_cpp_test(
    NAME swaps_test
    SRCS
      "swaps_tests.cc"
    DEPS
        ql::swaps
        GTest::gtest_main
)