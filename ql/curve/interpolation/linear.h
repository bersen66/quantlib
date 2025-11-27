#pragma once

#include <ql/curve/curve.h>

namespace ql {

struct Linear {

    static constexpr bool kStatefulImplementation = false;

    static ql::Percent Interpolate(const Curve::PointsContainer& points, const DateType& date);

};

} // namespace ql