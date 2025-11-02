#pragma once

#include <ql/types/types.h>
#include <ql/base/generator.h>
#include <chrono>
#include <optional>
#include <algorithm>

namespace ql {

class SwapBuilder;

class IrsSwap {
public:
    friend class SwapBuilder;

    struct FixedLeg {
        DateType date;
    };

    struct FloatLeg {
        DateType date;
        std::optional<f64> pay;
        std::optional<f64> recv;
    };


    void FloatLegAddPay(const DateType& at, Percent ruoni);

    f64 FixedPayOrRecv() const {
        return fixed_payment;
    }

private:


    IrsSwap(Percent coupon, f64 notional, const DateType& effd, const DateType& matd, bool fix_paying);

private:
    std::vector<FixedLeg> fxl;
    std::vector<FloatLeg> fll;
    DateType effective_date;
    DateType maturity_date;
    Percent coupon;
    f64 fixed_payment;
    f64 notional;
    bool fix_paying;
};


class SwapBuilder {
public:

    SwapBuilder()
        : coupon(std::nullopt)
        , notional(std::nullopt)
    {}

    inline SwapBuilder& Coupon(Percent p) {
        coupon = p;
        return *this;
    }

    inline SwapBuilder& Notional(f64 value) {
        notional = value;
        return *this;
    }

    // Starting date
    inline SwapBuilder& EffectiveDate(const DateType& edt) {
        effective_date = edt;
        return *this;
    }

    // Expiration date
    inline SwapBuilder& MaturityDate(const DateType& mdt) {
        maturity_date = mdt;
        return *this;
    }

    inline SwapBuilder& FloatingLegPay(const DateType& date, f64 pay) {
        if (!float_payments) {
            float_payments = std::vector<IrsSwap::FloatLeg>();
            float_payments->reserve(8);
        }
        auto it = std::ranges::find(float_payments.value(), date, &IrsSwap::FloatLeg::date);
        if (it == float_payments->end()) {
            float_payments->emplace_back(date, pay, std::nullopt);
        } else {
            it->pay = pay;
        }

        return *this;
    }

    inline SwapBuilder& FloatingLegRecv(const DateType& date, f64 recv) {
        if (!float_payments) {
            float_payments = std::vector<IrsSwap::FloatLeg>();
            float_payments->reserve(8);
        }
        auto eq_date = [&date] (const IrsSwap::FloatLeg& fl) {
            return fl.date == date;
        };

        auto it = std::ranges::find(float_payments.value(), date, &IrsSwap::FloatLeg::date);
        if (it == float_payments->end()) {
            float_payments->emplace_back(date, std::nullopt, recv);
        } else {
            it->recv = recv;
        }

        return *this;
    }


    inline SwapBuilder& FixedLegAddDate(const DateType& date) {
        if (!fix_payments) {
            fix_payments = std::vector<IrsSwap::FixedLeg>();
            fix_payments->reserve(8);
        }

        auto it = std::ranges::find(fix_payments.value(), date, &IrsSwap::FixedLeg::date);
        if (it == fix_payments->end()) {
            fix_payments->emplace_back(date);
        }

        return *this;
    }


    inline SwapBuilder& WePayingFix(bool val) {
        paying_fix = val;
        return *this;
    }

    void Reset() {
        float_payments = std::nullopt;
        fix_payments = std::nullopt;
        notional = std::nullopt;
        coupon = std::nullopt;
        effective_date = std::nullopt;
        maturity_date = std::nullopt;
        paying_fix = std::nullopt;
    }

    inline IrsSwap BuildIrsSwap() {
        namespace stdr = std::ranges;

        assert(float_payments.has_value());
        assert(fix_payments.has_value());
        assert(coupon.has_value());
        assert(notional.has_value());
        assert(paying_fix.has_value());

        fix_payments->emplace_back(maturity_date.value());
        float_payments->emplace_back(maturity_date.value());

        stdr::sort(float_payments.value(), std::less<>{}, &IrsSwap::FloatLeg::date);
        stdr::sort(fix_payments.value(), std::less<>{}, &IrsSwap::FixedLeg::date);

        auto res = IrsSwap(coupon.value(), notional.value(), effective_date.value(), maturity_date.value(), paying_fix.value());

        // Fixed counterpart computation
        auto d1 = (std::chrono::sys_days(fix_payments.value()[1].date) - std::chrono::sys_days(fix_payments.value()[0].date)).count();
        res.fixed_payment = coupon->Apply(notional.value()) * d1  / 360;

        res.fxl = std::move(fix_payments).value();
        res.fll = std::move(float_payments).value();

        Reset();
        return res;
    }

private:
    std::optional<std::vector<IrsSwap::FloatLeg>> float_payments;
    std::optional<std::vector<IrsSwap::FixedLeg>> fix_payments;
    std::optional<DateType> effective_date;
    std::optional<DateType> maturity_date;
    std::optional<Percent> coupon; // receive fixed
    std::optional<f64> notional; // Whole sum
    std::optional<bool> paying_fix;
};

}  // namespace ql
