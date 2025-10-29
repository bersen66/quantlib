#pragma once

#include <ql/types/types.h>
#include <ql/base/generator.h>
#include <chrono>
#include <optional>


namespace ql {

class SwapBuilder;

class IrsSwap {
public:
    using DateType = std::chrono::year_month_day;
public:
    friend class SwapBuilder;

    struct CacheFlow {
        f64 pv;
        f64 fv;
    };

    Generator<f64> FixedCounterpart();

    Generator<CacheFlow> FloatCounterpart();


private:

    IrsSwap(f64 coupon, f64 notional, const DateType& effd, const DateType& matd);

private:

    DateType effective_date;
    DateType maturity_date;
    f64 coupon;
    f64 notional;
    u32 fixed_payments_count;
    u32 float_payments_count;
};

class SwapBuilder {
public:
    using DateType = std::chrono::year_month_day;
public:

    SwapBuilder()
        : coupon(std::nullopt)
        , notional(std::nullopt)
    {}

    inline SwapBuilder& Coupon(f64 value) {
        coupon = value;
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

    void Reset() {
        notional = std::nullopt;
        coupon = std::nullopt;
        effective_date = std::nullopt;
        maturity_date = std::nullopt;
    }

    inline IrsSwap BuildIrsSwap() {
        auto res = IrsSwap(coupon.value(), notional.value(), effective_date.value(), maturity_date.value());
        Reset();
        return res;
    }


private:
    std::optional<DateType> effective_date;
    std::optional<DateType> maturity_date;
    std::optional<f64> coupon; // receive fixed
    std::optional<f64> notional; // Whole sum
};

}  // namespace ql