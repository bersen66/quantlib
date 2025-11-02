#include <gtest/gtest.h>
#include <ql/swaps/swaps.h>
#include <chrono>


TEST(Basic, Option) {
    using namespace std::chrono;
    using namespace ql::literals;

    // .clang-format off
    ql::IrsSwap irs = ql::SwapBuilder()
        .EffectiveDate(day(1)/month(January)/year(2025))
        .MaturityDate(day(1)/month(January)/year(2026))
        .Coupon(0.24_percents)
        .Notional(1'000'000)
        .FixedLegAddDate(day(1)/month(February)/year(2025))
        .FixedLegAddDate(day(1)/month(April)/year(2025))
        .FixedLegAddDate(day(1)/month(July)/year(2025))
        .FixedLegAddDate(day(1)/month(October)/year(2025))
        .BuildIrsSwap()
    ;
    // .clang-format on


}
