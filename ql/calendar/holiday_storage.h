#pragma once

#include <ql/calendar/date.h>
#include <ql/calendar/freq.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <set>
#include <string>
#include <unordered_map>

namespace ql {

class HolidayStorage {
private:
    using StorageType = std::unordered_map<std::string, std::set<DateType>>;

public:
    HolidayStorage() = default;

    HolidayStorage(const HolidayStorage&) = delete;
    HolidayStorage& operator=(const HolidayStorage) = delete;

    ~HolidayStorage() = default;

    inline void Insert(const std::string& jur, const DateType& date) {
        storage[jur].emplace(date);
    }

    bool IsWeekend(const std::string& jur, const DateType& date) const;

    template <std::input_iterator InputIt>
    inline bool IsWeekendEachJur(const DateType& date, InputIt jur_begin, InputIt jur_end) const {
        static_assert(std::is_convertible_v<std::iter_value_t<InputIt>, std::string>,
                      "Expected iterators to Jurisdictions");

        return std::all_of(jur_begin, jur_end, [this, &date](const std::string& jur) { return IsWeekend(jur, date); });
    }

    template <std::input_iterator InputIt>
    inline bool IsWorkdayEachJur(const DateType& date, InputIt jur_begin, InputIt jur_end) const {
        static_assert(std::is_convertible_v<std::iter_value_t<InputIt>, std::string>,
                      "Expected iterators to Jurisdictions");

        return std::none_of(jur_begin, jur_end, [this, &date](const std::string& jur) { return IsWeekend(jur, date); });
    }

    template <std::input_iterator InputIt>
    bool AreWorkdays(const std::string& jur, InputIt days_begin, InputIt days_end) const {
        static_assert(std::is_convertible_v<std::iter_value_t<InputIt>, DateType>, "Expected iterators to dates");

        return std::all_of(days_begin, days_end, [this, &jur](const DateType& date) { return IsWeekend(jur, date); });
    }

    template <std::input_iterator InputIt>
    inline bool AreWeekends(const std::string& jur, InputIt days_begin, InputIt days_end) const {
        static_assert(std::is_convertible_v<std::iter_value_t<InputIt>, DateType>, "Expected iterators to dates");

        return std::none_of(days_begin, days_end, [this, &jur](const DateType& date) { return IsWeekend(jur, date); });
    }

    template <std::bidirectional_iterator DateIter, std::input_iterator JurIter>
    inline bool AreWorkdaysEachJur(DateIter date_begin, DateIter date_end, JurIter jur_begin, JurIter jur_end) {
        static_assert(std::is_convertible_v<std::iter_value_t<DateIter>, DateType>, "Expected iterators to dates");
        static_assert(std::is_convertible_v<std::iter_value_t<JurIter>, std::string>,
                      "Expected iterators to Jurisdictions");

        return std::all_of(jur_begin, jur_end,
                           [&](const std::string& jur) { return AreWorkdays(jur, date_begin, date_end); });
    }

    [[nodiscard]] DateType FindNextWorkingDay(const std::string& jur, const DateType& date) const;

    [[nodiscard]] DateType FindPreviousWorkingDay(const std::string& jur, const DateType& date) const;

    bool Empty() const noexcept {
        return storage.empty();
    }

    void Clear() {
        return storage.clear();
    }

    struct HolidayStorageDeclarativeInit {
        HolidayStorageDeclarativeInit& operator()(const std::string& jur, const DateType& date) {
            parent->Insert(jur, date);
            return *this;
        }

        HolidayStorage* parent;
    };

    HolidayStorageDeclarativeInit StaticInit() {
        return HolidayStorageDeclarativeInit{this};
    }

    Generator<DateType> BuisnessDays(Generator<DateType> dates, const std::string& jur,
                                     Adjustment adjustment = Adjustment::kFollowing) const;

private:
    const std::set<DateType>& JurisdictionHolidays(const std::string& jur) const;

    DateType AdjustWorkDay(const std::string& jur, DateType date, Adjustment adj) const;
private:
    StorageType storage;
};

}  // namespace ql
