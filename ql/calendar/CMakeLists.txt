ql_cpp_library(
    NAME calendar
    HDRS
      "holiday_storage.h"
      "date.h"
      "freq.h"
    SRCS
      "holiday_storage.cc"
      "date.cc"
    PUBLIC
)

ql_cpp_test(
    NAME test_date
    SRCS
      "holiday_storage_test.cc"
      "date_test.cc"
    DEPS
      ql::calendar
      GTest::gtest_main
      GTest::gmock
)