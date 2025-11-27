#include <ql/calendar/date.h>
#include <stdexcept>

namespace chrono = std::chrono;

namespace ql {

DateType NextDay(const DateType& ymd) {
    chrono::sys_days days = ymd;
    days += chrono::days(1);
    return chrono::year_month_day(days);
}

DateType PreviousDay(const DateType& ymd) {
    chrono::sys_days days = ymd;
    days -= chrono::days(1);
    return chrono::year_month_day(days);
}

WeekDayType Weekday(const DateType& ymd) {
    chrono::sys_days sd(ymd);
    return chrono::weekday(sd);
}

int DayDifference(const DateType& lhs, const DateType& rhs) {
    return (chrono::sys_days(lhs) - chrono::sys_days(rhs)).count();
}

unsigned LastMonthDay(const DateType& ymd) {
    static constexpr std::array<unsigned, 12> normal_end_dates = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    unsigned m = static_cast<unsigned>(ymd.month());
    return (m != 2 || !ymd.year().is_leap() ? normal_end_dates[m-1] : 29);
}

bool IsLastMonthDay(const DateType& ymd) {
    return static_cast<unsigned>(ymd.day()) == LastMonthDay(ymd);
}

void AddMonths(DateType& ymd, unsigned months) {
    ymd += chrono::months(months);

    if (!ymd.ok()) {
        ymd = ymd.year()/ymd.month()/chrono::day{LastMonthDay(ymd)};
    }
}

DateType NextYearBeginnning(const DateType& date) {
    return date.year()++/1/1;
}

Generator<DateType> Period::WithFrequency(Freq freq) const {
    DateType current_date = since;

    switch (freq) {
    case Freq::kAnnualy:
        while (current_date <= until) {
            co_yield current_date;
            AddMonths(current_date, 12);
        }
        break;

    case Freq::kSemiAnnualy:
        while (current_date <= until) {
            co_yield current_date;
            AddMonths(current_date, 6);
        }
        break;

    case Freq::kQuarterly:
        while (current_date <= until) {
            co_yield current_date;
            AddMonths(current_date, 3);
        }
        break;

    case Freq::kMonthly:
        while (current_date <= until) {
            co_yield current_date;
            AddMonths(current_date, 1);
        }
        break;

    case Freq::kDaily:
        while (current_date <= until) {
            co_yield current_date;
            current_date = NextDay(current_date);
        }
        break;
    }
}

Period::Period(const DateType& since, const DateType& until) 
    : since(since), until(until)
{
    if (!Valid()) {
        throw std::logic_error("invalid period");
    }
}

f64 Period::ActActISDA() const {   
    if (SameYear()) {
        return static_cast<f64>(Days()) / DaysInYear(since);
    }

    f64 leap_days = 0;
    f64 non_leap_days = 0;

    DateType tmp = since;
    while (true) {
        if (tmp > until) break;

        u32 difference = 0;
        if (tmp.year() + chrono::years(1) > until.year()) {
            difference = DayDifference(until, tmp);
        } else {
            difference = DaysTillTheEndOfYear(tmp);
        }
        
        if (tmp.year().is_leap()) {
            leap_days += difference;
        } else {
            non_leap_days += difference;
        }

        tmp = NextYearBeginnning(tmp);
    }

    return leap_days / 366.0 + non_leap_days / 365.0;
}

} // namespace ql
