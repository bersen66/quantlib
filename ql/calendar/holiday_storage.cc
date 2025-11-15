#include <ql/calendar/holiday_storage.h>

#include <chrono>
#include <stdexcept>

namespace ql {
const std::set<DateType>& HolidayStorage::JurisdictionHolidays(const std::string& jur) const {
    auto it = storage.find(jur);
    if (it == storage.end()) {
        throw std::runtime_error("unknown jurisdiction");
    }

    return it->second;
}

bool HolidayStorage::IsWeekend(const std::string& jur, const DateType& date) const {
    if (JurisdictionHolidays(jur).contains(date)) {
        return true;
    }

    WeekDayType wd = Weekday(date);
    return wd == std::chrono::Saturday || wd == std::chrono::Sunday;
}

DateType HolidayStorage::FindNextWorkingDay(const std::string& jur, const DateType& date) const {
    DateType result = date;

    do {
        result = NextDay(result);
    } while (IsWeekend(jur, result));

    return result;
}

DateType HolidayStorage::FindPreviousWorkingDay(const std::string& jur, const DateType& date) const {
    DateType result = date;

    do {
        result = NextDay(result);
    } while (IsWeekend(jur, result));

    return result;
}

DateType HolidayStorage::AdjustWorkDay(const std::string& jur, DateType date, Adjustment rule) const {
    if (!IsWeekend(jur, date)) {
        return date;
    }

    switch (rule) {
    case Adjustment::kFollowing:
        return FindNextWorkingDay(jur, date);
    case Adjustment::kPreceding:
        return FindPreviousWorkingDay(jur, date);
    case Adjustment::kModifiedFollowing: {
        DateType adjusted = FindNextWorkingDay(jur, date);
        if (adjusted.month() != date.month()) {
            return FindPreviousWorkingDay(jur, date);
        }
        return adjusted;
    }
    case Adjustment::kUnadjusted:
        return date;
    }

    return date;
}

Generator<DateType> HolidayStorage::BuisnessDays(Generator<DateType> dates, const std::string& jur,
                                                 Adjustment adjustment) const {
    DateType prev;
    for (DateType date : dates) {
        DateType adjusted = AdjustWorkDay(jur, date, adjustment);
        if (adjusted == prev) {
            continue;
        }
        co_yield adjusted;
        prev = adjusted;
    }
}

}  // namespace ql
