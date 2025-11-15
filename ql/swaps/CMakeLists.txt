ql_cpp_library(
    NAME swaps
    HDRS "irs.h"
    SRCS "irs.cc"
    DEPS 
      ql::types
      ql::calendar
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