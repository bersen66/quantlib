#pragma once

#include <date.h>
#include <ql/types/floats.h>

#include <functional>

namespace ql {

inline f64 Actual360(const DateType& lhs, const DateType& rhs) {
    return static_cast<f64>(DayDifference(lhs, rhs)) / 360.0;
}

inline f64 Actual365(const DateType& lhs, const DateType& rhs) {
    return static_cast<f64>(DayDifference(lhs, rhs)) / 365.0;
}

}  // namespace ql