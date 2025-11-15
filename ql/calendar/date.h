#pragma once

#include <ql/base/generator.h>
#include <ql/calendar/freq.h>

#include <chrono>

using DateType = std::chrono::year_month_day;
using WeekDayType = std::chrono::weekday;

namespace ql {

DateType NextDay(const DateType& date);

DateType PreviousDay(const DateType& ymd);

WeekDayType Weekday(const DateType& ymd);

int DayDifference(const DateType& lhs, const DateType& rhs);

unsigned LastMonthDay(const DateType& date);

bool IsLastMonthDay(const DateType& date);

class Period {
public:

    Period(const DateType& since, const DateType& until) : since(since), until(until) {
    }

    [[nodiscard]] bool Valid() const {
        return since <= until;
    }

    [[nodiscard]] Generator<DateType> WithFrequency(Freq freq) const;

    [[nodiscard]] const DateType& Since() const noexcept {
        return since;
    }
    [[nodiscard]] const DateType& Until() const noexcept {
        return until;
    }

private:
    DateType since;
    DateType until;
};

}  // namespace ql