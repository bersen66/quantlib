#pragma once

#include <ql/types/floats.h>
#include <ql/types/concepts.h>
#include <cmath>
#include <stdexcept>

namespace ql {

class Percent {
private:
    static struct percentage_tag_t {} percentage_tag;
    static struct fraction_tag_t {} fraction_tag;

public:

    inline static Percent Zero() {
        return Percent(fraction_tag, 0.0);
    }

    inline static Percent Hundred() {
        return Percent(fraction_tag, 1.0);
    }

    inline static Percent FromFraction(f64 val) {
        return Percent(fraction_tag, val);
    }

    inline static Percent FromPercentage(f64 val) {
        return Percent(percentage_tag, val);
    }


    inline f64 Percentage() const noexcept {
        return value * 100.0;
    }

    inline f64 Fraction() const noexcept {
        return value;
    }


    template<Numeric T>
    auto Apply(T amount) const -> T {
        if constexpr (std::integral<T>) {
            return static_cast<T>(std::round(amount * value));
        } else {
            return static_cast<T>(amount * value);
        }
    }


    Percent operator+() const noexcept {
        return *this;
    }

    Percent operator-() const noexcept {
        return Percent::FromFraction(-value);
    }

    Percent operator+(const Percent& other) const noexcept {
        return Percent::FromFraction(value + other.value);
    }

    Percent operator-(const Percent& other) const noexcept {
        return Percent::FromFraction(value - other.value);
    }

    Percent operator*(const Percent& other) const noexcept {
        return Percent::FromFraction(value * other.value);
    }

    Percent operator/(const Percent& other) const {
        if (other.value == 0.0) {
            throw std::runtime_error("Division by zero in Percent");
        }
        return Percent::FromFraction(value / other.value);
    }

    Percent& operator+=(const Percent& other) noexcept {
        value += other.value;
        return *this;
    }

    Percent& operator-=(const Percent& other) noexcept {
        value -= other.value;
        return *this;
    }

    Percent& operator*=(const Percent& other) noexcept {
        value *= other.value;
        return *this;
    }

    Percent& operator/=(const Percent& other) {
        if (other.value == 0.0) {
            throw std::runtime_error("Division by zero in Percent");
        }
        value /= other.value;
        return *this;
    }

    template<Numeric T>
    friend auto operator*(T scalar, const Percent& percent) -> T {
        return percent.Apply(scalar);
    }

    template<Numeric T>
    friend auto operator*(const Percent& percent, T scalar) -> T {
        return percent.Apply(scalar);
    }

    friend Percent operator*(const Percent& percent, f64 scalar) noexcept {
        return Percent::FromFraction(percent.value * scalar);
    }

    friend Percent operator*(f64 scalar, const Percent& percent) noexcept {
        return Percent::FromFraction(percent.value * scalar);
    }

    friend Percent operator/(const Percent& percent, f64 scalar) {
        if (scalar == 0.0) {
            throw std::runtime_error("Division by zero in Percent");
        }
        return Percent::FromFraction(percent.value / scalar);
    }

    Percent& operator*=(f64 scalar) noexcept {
        value *= scalar;
        return *this;
    }

    Percent& operator/=(f64 scalar) {
        if (scalar == 0.0) {
            throw std::runtime_error("Division by zero in Percent");
        }
        value /= scalar;
        return *this;
    }

    bool operator==(const Percent& other) const noexcept = default;

    auto operator<=>(const Percent& other) const noexcept {
        return value <=> other.value;
    }

    bool IsZero() const noexcept {
        return value == 0.0;
    }

    bool IsHundred() const noexcept {
        return value == 1.0;
    }

    bool IsPositive() const noexcept {
        return value > 0.0;
    }

    bool IsNegative() const noexcept {
        return value < 0.0;
    }

private:
    explicit Percent(percentage_tag_t, f64 val)
        : Percent(fraction_tag, val / 100.0)
    {}

    explicit Percent(fraction_tag_t, f64 val)
        : value(val)
    {}

private:
    f64 value;
};


inline namespace literals {

Percent operator""_percents(unsigned long long int_value) {
    return Percent::FromPercentage(static_cast<f64>(int_value));
}


Percent operator""_percents(long double float_value) {
    return Percent::FromPercentage(static_cast<f64>(float_value));
}

} // namespace literals

} // namespace ql
