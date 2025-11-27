#pragma once

#include <ql/calendar/date.h>
#include <ql/calendar/freq.h>
#include <ql/calendar/holiday_storage.h>
#include <ql/types/types.h>

#include <span>

namespace ql {

class IrsBuilder;

class PaymentPeriodEntry {
public:
    static constexpr u8 kNotInitialized = 255;

public:
    friend class IrsBuilder;
    friend class IrsContract;

    explicit PaymentPeriodEntry(const DateType& date, const std::optional<f64>& payment = std::nullopt)
        : date_(date)
        , payment_(payment)
        , chrono_prev_idx_(kNotInitialized)
        , chrono_next_idx_(kNotInitialized)
    {}

    [[nodiscard]] const DateType& Date() const noexcept {
        return date_;
    }

    [[nodiscard]] bool ChronoFirstPayment() const noexcept {
        return chrono_prev_idx_ == kNotInitialized;
    }

    [[nodiscard]] bool ChronoLastPayment() const noexcept {
        return chrono_next_idx_ == kNotInitialized;
    }

    [[nodiscard]] bool HasKnownPayment() const noexcept {
        return payment_.has_value();
    }

private:
    DateType date_;
    std::optional<f64> payment_;
    u8 chrono_prev_idx_;
    u8 chrono_next_idx_;
};

class IrsContract final {
public:

    class ChronologicalIterator final {
    public:

        ChronologicalIterator(PaymentPeriodEntry* buffer, u8 start_idx)
            : payment_periods_(buffer)
            , current_idx_(start_idx)
        {}

    private:
        PaymentPeriodEntry* payment_periods_;
        u8 current_idx_;
    };

public:
    friend class IrsBuilder;

    [[nodiscard]] std::span<const PaymentPeriodEntry> FixedLeg() const noexcept {
        return {fixed_leg_, float_leg_};
    }

    [[nodiscard]] std::span<const PaymentPeriodEntry> FloatLeg() const noexcept {
        return {float_leg_, payment_periods_.end().base()};
    }

    [[nodiscard]] const Percent& Coupon() const noexcept {
        return coupon_;
    }

    [[nodiscard]] bool PayFix() const noexcept {
        return paying_fix_;
    }
    //
    // ChronologicalIterator cbegin() const {
    //     return ChronologicalIterator(payment_periods_.data(), );
    // }

private:

    IrsContract(Percent coupon, bool paying_fix)
        : coupon_(coupon)
        , paying_fix_(paying_fix)
    {}

private:
    std::vector<PaymentPeriodEntry> payment_periods_;
    PaymentPeriodEntry* fixed_leg_ = nullptr;
    PaymentPeriodEntry* float_leg_ = nullptr;
    Percent coupon_;
    u8 chrono_start_idx_ = 0;
    u8 chrono_last_idx_ = 0;
    bool paying_fix_ = false;
};

class IrsBuilder final {
public:
    [[maybe_unused]] IrsBuilder& Coupon(Percent p) {
        cpn_ = p;
        return *this;
    }

    [[maybe_unused]] IrsBuilder& EffectiveDate(const DateType& ed) {
        effective_date_ = ed;
        return *this;
    }

    [[maybe_unused]] IrsBuilder& MaturityDate(const DateType& md) {
        maturity_date_ = md;
        return *this;
    }

    [[maybe_unused]] IrsBuilder& PayFix(bool pf) {
        paying_fix_ = pf;
        return *this;
    }

    [[maybe_unused]] IrsBuilder& FixedFreq(Freq freq) {
        fixed_freq_ = freq;
        return *this;
    }

    [[maybe_unused]] IrsBuilder& FloatFreq(Freq freq) {
        float_freq_ = freq;
        return *this;
    }

    [[maybe_unused]] IrsBuilder& Notion(f64 value) {
        notional_ = value;
        return *this;
    }

    [[nodiscard]] IrsContract Build(const HolidayStorage& hs, const std::string& jur,
                                    Adjustment adj = Adjustment::kFollowing);

private:
    std::optional<DateType> maturity_date_;
    std::optional<DateType> effective_date_;
    std::optional<Freq> fixed_freq_;
    std::optional<Freq> float_freq_;
    std::optional<Percent> cpn_;
    std::optional<f64> notional_;
    std::optional<bool> paying_fix_;
};

}  // namespace ql