#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ql/calendar/date.h>

using namespace std::chrono;

TEST(TestPeriod, sanity) {
    auto per = ql::Period(day(1) / January / 2025, day(10) / January / 2025);
    ASSERT_TRUE(per.Valid());
    ASSERT_EQ(per.Since(), day(1) / January / 2025) << "Definitely miss something";
    ASSERT_EQ(per.Until(), day(10) / January / 2025) << "Definitely miss something";

    std::vector<DateType> result;
    result.reserve(10);
    for (const DateType& date : per.WithFrequency(ql::Freq::kDaily)) {
        result.push_back(date);
    }

    EXPECT_THAT(result, testing::ElementsAreArray({
        day(1) / January / 2025,
        day(2) / January / 2025,
        day(3) / January / 2025,
        day(4) / January / 2025,
        day(5) / January / 2025,
        day(6) / January / 2025,
        day(7) / January / 2025,
        day(8) / January / 2025,
        day(9) / January / 2025,
        day(10) / January / 2025
    }));
}