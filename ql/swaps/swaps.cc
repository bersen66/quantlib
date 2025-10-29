#include <ql/swaps/swaps.h>

namespace ql {


Generator<f64> IrsSwap::FixedCounterpart() {
    using namespace std::chrono;
    u32 payments_num = 0;
    for (DateType i = effective_date + months(1); i <= maturity_date; i += months(1)) {
        payments_num++;
    }
    f64 cpn_factor = coupon / (100.0 * payments_num);
    std::cout << "Cpn factor = " << cpn_factor << std::endl;
    for (DateType payment_date = effective_date + months(1); payment_date <= maturity_date; payment_date += months(1)) {
        co_yield notional * cpn_factor;
    }
    co_return;
}

Generator<IrsSwap::CacheFlow> IrsSwap::FloatCounterpart() {
    // TODO
    effective_date += std::chrono::months(1);
    co_return;
}

IrsSwap::IrsSwap(f64 coupon, f64 notional, const DateType& effd, const DateType& matd)
    : effective_date(effd)
    , maturity_date(matd)
    , coupon(coupon)
    , notional(notional)
{}

} // namespace ql