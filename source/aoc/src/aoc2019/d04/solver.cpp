#include "aoc/solver.h"

#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <limits>
#include <type_traits>
#include <memory>
#include <cassert>
#include <functional>

namespace {
    using namespace aoc;

    constexpr std::size_t YEAR = 2019;
    constexpr std::size_t DAY  = 4;
    using input_t = std::pair<std::uintmax_t, std::uintmax_t>;
    template<std::size_t N> using digits_t = std::array<std::uint8_t, N>;

    input_t parse_input(std::istream& is)
    {
        input_t input;
        std::regex re{ R"_((\d+)-(\d+))_" };
        std::string line;
        while (std::getline(is, line)) {
            if (line.empty()) continue;
            if (std::smatch m; std::regex_match(line, m, re)) {
                input.first  = std::stoull(m[1].str());
                input.second = std::stoull(m[2].str());
                if (input.first  >= 1'000'000) throw parse_exception{};
                if (input.second >= 1'000'000) throw parse_exception{};
                if (input.first  <    100'000) throw parse_exception{};
                if (input.second <    100'000) throw parse_exception{};

                break;
            }
            throw parse_exception{};
        }
        while (std::getline(is, line)) if (!line.empty()) throw parse_exception{};

        return input;
    }

    template<typename It> constexpr bool is_non_decreasing(It start, It last) noexcept
    {
        return std::adjacent_find(start, last, std::greater{}) == last;
    }

    template<typename It> constexpr bool contains_duplets(It start, It last) noexcept
    {
        return std::adjacent_find(start, last) != last;
    }

    template<typename It> constexpr bool contains_unique_duplets(It start, It last) noexcept
    {
        while (true) {
            auto it = std::adjacent_find(start, last, std::not_equal_to{});
            if (it == last) return std::distance(start, it) == 2;
            ++it;
            if (std::distance(start, it) == 2) return true;
            start = it;
        }
        return false;
    }

    template<std::size_t N>
    auto to_digits(std::uintmax_t x) -> digits_t<N>
    {
        digits_t<N> digits;
        for (std::size_t i = 0; i < N; ++i) {
            digits[N-i-1] = x % 10;
            x /= 10;
        }
        return digits;
    }

    template<std::size_t N>
    auto from_digits(const digits_t<N>& ds) -> std::uintmax_t
    {
        std::uintmax_t x = 0;
        for (std::size_t i = 0; i < N; ++i) {
            x *= 10;
            x += ds[i];
        }
        return x;
    }

    std::size_t passwords_A_between(std::uintmax_t lower, std::uintmax_t upper) noexcept
    {
        std::size_t accum = 0;
        for (std::uintmax_t x = lower; x < upper; ++x) {
            if (auto ds = to_digits<6>(x); is_non_decreasing(ds.begin(), ds.end()) && contains_duplets(ds.begin(), ds.end())) ++accum;
        }
        return accum;
    }

    std::size_t passwords_B_between(std::uintmax_t lower, std::uintmax_t upper) noexcept
    {
        std::size_t accum = 0;
        for (std::uintmax_t x = lower; x < upper; ++x) {
            if (auto ds = to_digits<6>(x); is_non_decreasing(ds.begin(), ds.end()) && contains_unique_duplets(ds.begin(), ds.end())) ++accum;
        }
        return accum;
    }



    // constexpr std::size_t total_non_decreasing(std::uint8_t base, std::uint8_t digits) noexcept
    // {
    //     // F(b, d) = F(b, d-1) + F(b-1, d-1) + F(b-2, d-1) + ... + F(1, d-1)
    //     // F(1, d) = 1
    //     // F(b, 1) = b
    //     assert(base != 0 && digits != 0);
    //     if (digits == 1) return base;

    //     std::size_t accum = 0;
    //     for (std::uint8_t b = 1; b <= base; ++b) {
    //         accum += total_non_decreasing(b, digits-1);
    //     }
    //     return accum;
    // }

    // constexpr std::size_t total_nplets(std::uint8_t N, std::uint8_t base, std::uint8_t digits) noexcept
    // {
    //     assert(N > 1);
    //     if (digits < N) return 0;
    //     if (digits == N) return base;
    //     std::size_t accum = 0;
    //     // partition the numbers between:
    //     //  a) the first digit belongs to a sequence of at least n repetitions
    //     for (std::uint8_t b = 1; b <= base; ++b)
    //         accum += total_non_decreasing(b, digits-N);
    //     //  b) the first digit does not belong to a sequence of at least n repetitions
    //     for (std::uint8_t b = 1; b < base; ++b)
    //         accum += total_nplets(N, b, digits-1);
    //     return accum;
    // }


    // constexpr std::size_t total_increasing(std::uint8_t base, std::uint8_t digits) noexcept
    // {
    //     assert(base != 0 && digits != 0);
    //     if (digits == 1) return base;

    //     std::size_t accum = 0;
    //     for (std::uint8_t b = 1; b < base; ++b)
    //         accum += total_increasing(b, digits-1);
    //     return accum;
    // }


    // static_assert(10 == total_non_decreasing(10, 1), "failed assert");
    // static_assert( 1 == total_non_decreasing(1, 50), "failed assert");
    // static_assert(10 == total_non_decreasing(3, 3), "failed assert");
    // static_assert( 1 == total_increasing(3, 3), "failed assert");
    // static_assert( 0 == total_increasing(2, 3), "failed assert");
    // static_assert( 3 == total_increasing(3, 2), "failed assert");


    // template<std::size_t N> constexpr auto total_non_decreasing_less_than(const digits_t<N>& upper) noexcept -> std::size_t
    // {
    //     std::size_t accum = 0;
    //     std::uint8_t start = 0;
    //     for (std::size_t d = 0; d < N; ++d) {
    //         if (start > upper[d]) break;
    //         for (std::uint8_t x = start; x < upper[d]; ++x)
    //             accum += total_non_decreasing(10-x, std::uint8_t(N - d - 1));
    //         start = upper[d];
    //     }
    //     return accum;
    // }

    // template<typename It>
    // constexpr auto biggest_nplet(It start, It end) -> std::size_t
    // {
    //     std::size_t max_value = 0;
    //     while (start != end) {
    //         auto it = std::next( std::adjacent_find(it, std::prev(end), std::not_equal_to{}) );
    //         max_value = std::max(max_value, std::size_t(std::distance(start, it) - 1));
    //         start = it;
    //     }
    //     return max_value;
    // }

    // template<std::size_t D> constexpr auto total_nplets_less_than(const std::uint8_t N, const digits_t<D>& upper) noexcept -> std::size_t
    // {
    //     std::size_t accum = 0;

    //     // first digit
    //     for (std::uint8_t x = 0; x < upper[0]; ++x) {
    //         accum += total_non_decreasing(10-x, D-N);
    //         accum += total_nplets(N, 10-x-1, D-1);
    //     }

    //     // split the number between a prefix, current and suffix
    //     // with `d` the position of current
    //     std::uint8_t start = upper[0];
    //     for (std::size_t d = 1; d < D; ++d) {
    //         if (start > upper[d]) break;

    //         std::uint8_t suffix_digits = std::uint8_t(D - d - 1);

    //         // first of all, split prefix into two regions, one whose ending is a particular digit,
    //         // and the rest: [prefix_start, prefix_split) and [prefix_split, upper.rend)
    //         auto prefix_start = std::next(upper.rbegin(), suffix_digits + 1);
    //         auto prefix_split = std::next(std::adjacent_find(prefix_start, std::prev(upper.rend()), std::not_equal_to{}));

    //         // get the current nplet
    //         auto curr_nplet = std::size_t(std::distance(prefix_start, prefix_split) - 1);

    //         // check if we already fulfill the condition
    //         if (curr_nplet >= N) {
    //             // any non-decreasing combination of numbers from this point onwards is a valid nplet
    //             for (; d < N; ++d) {
    //                 if (start > upper[d]) break;
    //                 for (std::uint8_t x = start; x < upper[d]; ++x)
    //                     accum += total_non_decreasing(10-x, suffix_digits);
    //                 start = upper[d];
    //             }
    //             break;
    //         }

    //         // current digit starts at the same value of previous one, and so belongs
    //         // to a pre-constructed nplet, check this case differently

    //         // the first digit belongs to the curr_nplet
    //         std::uint8_t missing_to_nplet = N - curr_nplet - 1;
    //         if (missing_to_nplet == 0) { // we already got an n-plet
    //             accum += total_non_decreasing(10-start, suffix_digits);
    //         } else if (missing_to_nplet <= suffix_digits) {
    //             // split into two cases:
    //             //   a) current digit belongs to an nplet of N
    //             accum += total_non_decreasing(10-start, suffix_digits - missing_to_nplet);

    //             //   b) current digit does not belong to an nplet
    //             accum += total_nplets(N, 10 - start - 1, suffix_digits);
    //         }

    //         // the first digit does not belong to the curr_nplet
    //         for (std::uint8_t x = start + 1; x < upper[d]; ++x) {
    //             accum += total_nplets(N, 10-x, suffix_digits - 1);
    //         }

    //         start = upper[d];
    //     }
    //     return accum;
    // }

    // template<std::size_t N> constexpr auto total_increasing_less_than(const digits_t<N>& upper) -> std::size_t
    // {
    //     std::size_t accum = 0;
    //     std::uint8_t start = 0;
    //     for (std::size_t d = 0; d < N; ++d) {
    //         if (start > upper[d]) break;
    //         for (std::uint8_t x = start; x < upper[d]; ++x)
    //             accum += total_increasing(10-x-1, std::uint8_t(N - d - 1));
    //         start = upper[d]+1;
    //     }
    //     return accum;
    // }


    auto resultA(const input_t& in) noexcept -> std::size_t
    {
        return passwords_A_between(in.first, in.second);
    }

    auto resultB(const input_t& in) noexcept -> std::size_t
    {
        // auto x = 112'225;
        // auto ds = to_digits<6>(x);
        // return contains_unique_duplets(ds.begin(), ds.end());
        return passwords_B_between(in.first, in.second);
    }
}

namespace aoc {
    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}