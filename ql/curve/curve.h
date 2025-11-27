#pragma once

#include <ql/calendar/date.h>
#include <ql/types/integers.h>
#include <ql/types/percent.h>

#include <map>
#include <tuple>

namespace ql {

class Curve final {
public:
    using PointsContainer = std::map<DateType, Percent>;
public:
    Curve() = default;

    template <std::input_iterator It>
    Curve(It begin, It end) {
        auto it = points_.begin();
        for (; begin != end; ++begin) {
            const auto& [date, value] = *begin;
            it = points_.emplace_hint(it, date, value);
        }
    }

    struct CurveEasyInit {
        CurveEasyInit& operator()(DateType date, Percent value) {
            parent_->Insert(date, value);
            return *this;
        }

        Curve* parent_;
    };

    CurveEasyInit StaticInit() {
        return CurveEasyInit{this};
    }

    void Clear() {
        points_.clear();
    }

    template <typename Interpolation, typename... Args>
    [[nodiscard]] Percent Interpolated(DateType date, Args&&... args) {

        if constexpr (Interpolation::kStatefulImplementation) {
            auto args_tuple = std::forward_as_tuple(std::forward<Args>(args)...);

            static_assert(sizeof...(Args) >= 1, "Stateless implementations must provide state");
            auto&& state = std::get<0>(args_tuple);

            if constexpr (sizeof...(Args) > 1) {
                return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    return state.Interpolate(points_, date,
                                             std::get<Is + 1>(std::forward_as_tuple(std::forward<Args>(args)...))...);
                }(std::make_index_sequence<sizeof...(Args) - 1>{});
            } else {
                return state.Interpolate(points_, date);
            }

        } else {
            return Interpolation::Interpolate(points_, date, std::forward<Args>(args)...);
        }
    }

private:
    void Insert(DateType when, Percent value) {
        points_[when] = value;
    }

private:
    PointsContainer points_;
};

}  // namespace ql