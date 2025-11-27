ql_cpp_library(
    NAME pricing
    HDRS
      "curve.h"
    SRCS
      "curve.cc"
    DEPS
        ql::calendar
    PUBLIC
)
