#include <ql/swaps/swaps.h>


namespace ql {

IrsSwap::IrsSwap(Percent coupon, f64 notional, const DateType& effd, const DateType& matd, bool fix_paying)
    : effective_date(effd)
    , maturity_date(matd)
    , coupon(coupon)
    , notional(notional)
    , fix_paying(fix_paying)
{
}

void IrsSwap::FloatLegAddPay(const DateType& at, Percent ruoni) {
    auto it = std::ranges::lower_bound(fll, at, std::less<>{}, &FloatLeg::date);
    assert(it != fll.end());
    assert(it->date == at);
    f64 dc_fraction = 0;
    if (it == fll.begin()) {
        auto d1 = (std::chrono::sys_days(it->date) - std::chrono::sys_days(maturity_date)).count();
        dc_fraction = d1 / 360;
    } else {
        auto d1 = (std::chrono::sys_days(it->date) - std::chrono::sys_days(std::prev(it)->date)).count();
        dc_fraction = d1 / 360;
    }

    f64 pmnt = ruoni.Apply(notional) * dc_fraction;
    if (fix_paying) {
        assert(!it->recv.has_value());
        it->recv = pmnt;
    } else {
        assert(!it->recv.has_value());
        it->recv = pmnt;
    }
}

} // namespace ql
