#include <gtest/gtest.h>
#include <chrono>

#include <ql/swaps/irs.h>

TEST(Basic, Option) {
    using namespace std::chrono;
    using namespace ql::literals;

    ql::HolidayStorage holiday_storage;

    holiday_storage.StaticInit()
        ("RUS", year(2025)/January/day(1))
        ("RUS", year(2025)/January/day(2))
        ("RUS", year(2025)/January/day(3))
        ("RUS", year(2025)/January/day(4))
        ("RUS", year(2025)/January/day(5))
        ("RUS", year(2025)/January/day(6))
        ("RUS", year(2025)/January/day(7))
        ("RUS", year(2025)/January/day(8))
        ("RUS", year(2025)/January/day(9))
    ;

    ql::IrsContract irs = ql::IrsBuilder()
        .Coupon(ql::Percent::FromPercentage(0.24))
        .PayFix(true)
        .Notion(2'000'000)
        .FixedFreq(ql::Freq::kQuarterly)
        .FloatFreq(ql::Freq::kAnnualy)
        .MaturityDate(day(1)/January/year(2025))
        .EffectiveDate(day(1)/January/year(2023))
        .Build(holiday_storage, "RUS");

    for (const ql::PaymentPeriodEntry& payment : irs.FixedLeg()) {
        ASSERT_TRUE(payment.HasKnownPayment());
    }

    for (const ql::PaymentPeriodEntry& payment : irs.FloatLeg()) {
        ASSERT_FALSE(payment.HasKnownPayment());
    }
}
