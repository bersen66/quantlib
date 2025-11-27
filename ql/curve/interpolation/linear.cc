#include <ql/curve/interpolation/linear.h>

namespace ql {

ql::Percent Linear::Interpolate(const ql::Curve::PointsContainer& points, const DateType& date) {
    if (points.empty()) [[unlikely]] {
        return Percent::Zero();
    }

    auto lo_it = points.lower_bound(date);

    if (lo_it == points.end()) [[unlikely]] {
        return Percent::Zero(); // FarRight(points_);
    }

    if (lo_it->first == date) {
        return lo_it->second;
    }

    if (lo_it == points.begin()) [[unlikely]] {
        return Percent::Zero(); // FarLeft(points_);
    }

    auto up_it = points.upper_bound(date);

    const auto& [lo_date, lo_value] = *lo_it;
    const auto& [up_date, up_value] = *up_it;

    return Percent::Zero(); // Between(lo_date, lo_value, date, up_date, up_value);
}

}  // namespace ql