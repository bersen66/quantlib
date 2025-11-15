#include <ql/calendar/date.h>

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

} // namespace ql
