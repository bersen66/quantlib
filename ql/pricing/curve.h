#pragma once

#include <ql/calendar/date.h>
#include <ql/calendar/holiday_storage.h>
#include <ql/types/floats.h>
#include <map>

namespace ql {

class Curve {
public:

    explicit Curve(ql::HolidayStorage& cs);

    f64 InterpolatedRate(const std::string& jur, const DateType& date) const;

    struct CurveEasyInit {
        CurveEasyInit& operator()(const DateType& date, f64 value) {
            parent->rates_[date] = value;
        }

        Curve* parent;
    };

    CurveEasyInit StaticInit() {
        return CurveEasyInit{this};
    }

private:
    std::map<DateType, f64> rates_;
    ql::HolidayStorage& hs_;
};

} // namespace ql