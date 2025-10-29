#include <gtest/gtest.h>
#include <ql/swaps/swaps.h>
#include <chrono>


TEST(Basic, Option) {
    using namespace std::chrono;

    ql::IrsSwap irs = ql::SwapBuilder()
        .EffectiveDate(day(1)/month(January)/year(2025))
        .MaturityDate(day(1)/month(January)/year(2026))
        .Coupon(8)
        .Notional(1'000'000)
        .BuildIrsSwap()
    ;

    f64 sum = 0;
    for (f64 payment : irs.FixedCounterpart()) {
        std::cout << payment << std::endl;
        sum += payment;
    }

}