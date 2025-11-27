ql_cpp_library(
  NAME curve
  HDRS
    "curve.h"
    "interpolation/linear.h"
  SRCS
    "interpolation/linear.cc"
  DEPS
    ql::calendar
)