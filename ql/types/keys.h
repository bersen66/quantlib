#pragma once

#include <functional>  // for std::hash
#include <tuple>
#include <type_traits>

namespace ql {

namespace internal {

template <typename T, typename MemberPtr>
constexpr decltype(auto) Access(T&& obj, MemberPtr ptr) {
    if constexpr (std::is_pointer_v<std::remove_reference_t<T>>) {
        return (*obj).*ptr;
    } else {
        return obj.*ptr;
    }
}

template <typename T>
constexpr void CombineHashes(std::size_t& seed, const T& data) {
    std::hash<T> hasher;
    seed ^= hasher(data) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

}  // namespace internal

template <auto... Members>
struct Less {
    template <typename T>
    constexpr bool operator()(T&& lhs, T&& rhs) const {
        if constexpr (sizeof...(Members) == 0) {
            return lhs < rhs;  // 0 members -> Comparing whole struct
        } else if constexpr (sizeof...(Members) == 1) {
            return internal::Access(lhs, std::get<0>(std::tuple{Members...})) <
                   internal::Access(rhs, std::get<0>(std::tuple{Members...}));
        } else {
            return std::tie(internal::Access(lhs, Members)...) < std::tie(internal::Access(rhs, Members)...);
        }
    }
};

template <auto... Members>
struct LessEq {
    template <typename T>
    constexpr bool operator()(T&& lhs, T&& rhs) const {
        if constexpr (sizeof...(Members) == 0) {
            return lhs <= rhs;  // 0 members -> Comparing whole struct
        } else if constexpr (sizeof...(Members) == 1) {
            return internal::Access(lhs, std::get<0>(std::tuple{Members...})) <=
                   internal::Access(rhs, std::get<0>(std::tuple{Members...}));
        } else {
            return std::tie(internal::Access(lhs, Members)...) <= std::tie(internal::Access(rhs, Members)...);
        }
    }
};

template <auto... Members>
struct Greater {
    template <typename T>
    constexpr bool operator()(T&& lhs, T&& rhs) const {
        if constexpr (sizeof...(Members) == 0) {
            return lhs > rhs;
        } else if constexpr (sizeof...(Members) == 1) {
            return internal::Access(lhs, std::get<0>(std::tuple{Members...})) >
                   internal::Access(rhs, std::get<0>(std::tuple{Members...}));
        } else {
            return std::tie(internal::Access(lhs, Members)...) > std::tie(internal::Access(rhs, Members)...);
        }
    }
};

template <auto... Members>
struct GreaterEq {
    template <typename T>
    constexpr bool operator()(T&& lhs, T&& rhs) const {
        if constexpr (sizeof...(Members) == 0) {
            return lhs >= rhs;  // 0 members -> Comparing whole struct
        } else if constexpr (sizeof...(Members) == 1) {
            return internal::Access(lhs, std::get<0>(std::tuple{Members...})) >=
                   internal::Access(rhs, std::get<0>(std::tuple{Members...}));
        } else {
            return std::tie(internal::Access(lhs, Members)...) >= std::tie(internal::Access(rhs, Members)...);
        }
    }
};

template <auto... Members>
struct Equal {
    template <typename T>
    constexpr bool operator()(T&& lhs, T&& rhs) const {
        if constexpr (sizeof...(Members) == 0) {
            return lhs == rhs;
        } else if constexpr (sizeof...(Members) == 1) {
            return internal::Access(lhs, std::get<0>(std::tuple{Members...})) ==
                   internal::Access(rhs, std::get<0>(std::tuple{Members...}));
        } else {
            return std::tie(internal::Access(lhs, Members)...) == std::tie(internal::Access(rhs, Members)...);
        }
    }
};

template <auto... Members>
struct NotEqual {
    template <typename T>
    constexpr bool operator()(T&& lhs, T&& rhs) const {
        return !Equal<Members...>{}(std::forward<T>(lhs), std::forward<T>(rhs));
    }
};

template <auto... Members>
struct Hash {
    template <typename T>
    constexpr std::size_t operator()(T&& t) const {
        std::size_t seed = 0;
        if constexpr (sizeof...(Members) == 0) {
            internal::CombineHashes(seed, t); // 0 members -- compute whole hash
        } else {
            (internal::CombineHashes(seed, internal::Access(t, Members)), ...);
        }
        return seed;
    }
};

}  // namespace ql
