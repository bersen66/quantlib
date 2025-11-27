#pragma once

#include <ql/base/generator.h>
#include <ql/calendar/freq.h>
#include <ql/types/integers.h>
#include <ql/types/floats.h>
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

void AddMonths(DateType& ymd, unsigned months);

inline constexpr u64 DaysInYear(const DateType& date) {
    return date.year().is_leap() ? 366 : 365;
}

inline constexpr u32 DaysTillTheEndOfYear(const DateType& date) {
    if (date.year().is_leap()) {
        return 366 - static_cast<u32>(date.day());
    }
    return 365 - static_cast<u32>(date.day());
}

DateType NextYearBeginnning(const DateType& date);

class Period {
public:
    using DiffType = i32;
public:
    Period(const DateType& since, const DateType& until);

    [[nodiscard]] bool Valid() const {
        return since.ok() && until.ok() && since <= until;
    }

    [[nodiscard]] Generator<DateType> WithFrequency(Freq freq) const;

    [[nodiscard]] const DateType& Since() const noexcept {
        return since;
    }
    [[nodiscard]] const DateType& Until() const noexcept {
        return until;
    }

    [[nodiscard]] DiffType Days() const noexcept {
        return DayDifference(Since(), Until());
    }

    // DC factors:
    [[nodiscard]] f64 Act360() const noexcept {
        return static_cast<f64>(Days()) / 360;
    }

    [[nodiscard]] f64 Act365() const noexcept {
        return static_cast<f64>(Days()) / 365;
    }

    [[nodiscard]] f64 ActActISDA() const;

    bool Contains(const Period& other) const noexcept {
        return Since() <= other.Since() && Until() >= other.Until();
    }
    
    bool SameYear() const noexcept {
        return Since().year() == Until().year();
    }
private:
    DateType since;
    DateType until;
};

}  // namespace ql