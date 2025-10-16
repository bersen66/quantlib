ql_cpp_library(
    NAME dummy
    HDRS
      "dummy.h"
    SRCS
      "dummy.cc"
)

ql_cpp_test (
    NAME test_dummy
    SRCS
      "dummy_test.cc"
    DEPS
        ql::dummy
        GTest::gtest_main
)

