#include <ql/swaps/irs.h>

#include <queue>
#include <utility>

namespace ql {

[[nodiscard]] IrsContract IrsBuilder::Build(const HolidayStorage& hs, const std::string& jur, Adjustment adj) {
    assert(maturity_date_.has_value());
    assert(effective_date_.has_value());
    assert(cpn_.has_value());
    assert(notional_.has_value());
    assert(paying_fix_.has_value());

    IrsContract result(cpn_.value(), paying_fix_.value());
    std::vector<PaymentPeriodEntry> sched;
    sched.reserve(10); // TODO: small vector or precompute payments num
    auto period = Period(effective_date_.value(), maturity_date_.value());

    auto compare_dates = [&sched](u8 left_idx, u8 right_idx) {
        return sched[left_idx].Date() > sched[right_idx].Date();
    };
    std::priority_queue<u8, std::vector<u8>, decltype(compare_dates)> chronological_order(compare_dates);

    f64 fixed_payment = 0.0; // TODO: compute
    u8 idx = 0;
    for (DateType date : hs.BuisnessDays(period.WithFrequency(fixed_freq_.value()), jur, adj)) {
        sched.emplace_back(date, fixed_payment);
        chronological_order.push(idx++);
    }
    u64 fixed_last = sched.size();

    for (DateType date : hs.BuisnessDays(period.WithFrequency(float_freq_.value()), jur, adj)) {
        sched.emplace_back(date);
        chronological_order.push(idx++);
    }

    u8 last = PaymentPeriodEntry::kNotInitialized;
    u8 curr = PaymentPeriodEntry::kNotInitialized;
    while (!chronological_order.empty()) {
        last = std::exchange(curr, chronological_order.top());
        chronological_order.pop();

        sched[curr].chrono_prev_idx_ = last;

        if (last == PaymentPeriodEntry::kNotInitialized) [[unlikely]] {
            continue;
        }
        sched[last].chrono_next_idx_ = curr;
    }

    result.payment_periods_ = std::move(sched);
    result.fixed_leg_ = result.payment_periods_.data();
    result.float_leg_ = result.payment_periods_.data() + fixed_last;

    return result;
}

} // namespace ql