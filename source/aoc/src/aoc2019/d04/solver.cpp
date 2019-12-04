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


    constexpr std::size_t total_non_decreasing(std::uint8_t base, std::uint8_t digits) noexcept
    {
        // F(b, d) = F(b, d-1) + F(b-1, d-1) + F(b-2, d-1) + ... + F(1, d-1)
        // F(1, d) = 1
        // F(b, 1) = b
        if (digits == 0) return 0;
        if (base <= 1 || digits == 1) return base;

        std::size_t accum = 0;
        for (std::uint8_t b = 1; b <= base; ++b) {
            accum += total_non_decreasing(b, digits-1);
        }
        return accum;
    }

    constexpr std::size_t total_increasing(std::uint8_t base, std::uint8_t digits) noexcept
    {
        assert(base > 0 && digits >= 0);
        if (base == 1) return digits == 1;
        if (digits == 1) return base;

        std::size_t accum = 0;
        for (std::uint8_t b = 1; b < base; ++b)
            accum += total_increasing(b, digits-1);
        return accum;
    }

    constexpr std::size_t total_passwords(std::uint8_t base, std::uint8_t digits) noexcept
    {
        return total_non_decreasing(base, digits) - total_increasing(base, digits);
    }

    template<std::size_t N>
    auto to_digits(std::uintmax_t x) -> std::array<std::uint8_t, N>
    {
        std::array<std::uint8_t, N> digits;
        for (std::size_t i = 0; i < N; ++i) {
            digits[N-i-1] = x % 10;
            x /= 10;
        }
        return digits;
    }

    auto resultA(const input_t& in) noexcept -> std::size_t
    {
        constexpr std::uint8_t DIGITS = 6;
        auto first_digits = to_digits<DIGITS>(in.first);
        auto second_digits = to_digits<DIGITS>(in.second);

        if (auto it = std::adjacent_find(first_digits.begin(), first_digits.end(), std::greater{}); it != first_digits.end())
            std::fill(std::next(it), first_digits.end(), *it);
        if (auto it = std::adjacent_find(second_digits.begin(), second_digits.end(), std::greater{}); it != second_digits.end())
            std::fill(std::next(it), second_digits.end(), *it);

        auto passwords_below = [DIGITS](const std::array<std::uint8_t, DIGITS>& digits) {

            std::size_t amount = 0;
            for (std::uint8_t d = 0; d < DIGITS; ++d) {
                amount += digits[d] * total_passwords(9, DIGITS-1-d);
            }
            return amount + 1;
        };

        return passwords_below({0, 0, 0, 0, 2, 3});
    }

    auto resultB(const input_t& in) noexcept -> std::optional<std::size_t>
    {
        // ...
        return std::nullopt;
    }
}

namespace aoc {
    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}