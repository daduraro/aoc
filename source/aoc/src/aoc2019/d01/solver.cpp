#include "aoc/solver.h"
#include "aoc/error.h"
#include "version.h"

#include <iostream>
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

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = AOC_YEAR;
        constexpr std::size_t DAY  = AOC_DAY;
        using input_t = std::vector<std::size_t>;

        auto resultA(const input_t& in) noexcept -> std::size_t
        {
            return std::accumulate(in.begin(), in.end(), std::size_t(0), [](auto acc, auto x) {
                return acc + (x/3) - 2;
            });
        }

        auto resultB(const input_t& in) noexcept -> std::size_t
        {
            return (std::size_t) std::accumulate(in.begin(), in.end(), std::intmax_t(0), [](auto acc, std::intmax_t x) {
                x = x / 3 - 2;
                while (x >= 0) {
                    acc += x;
                    x = x / 3 - 2;
                }
                return acc;
            });
        }

        void parse_input(std::istream& is, input_t& in)
        {
            using input_it = std::istream_iterator<std::size_t>;
            std::copy(input_it(is), input_it(), std::back_inserter(in));

            if (!is.eof()) throw parse_exception{"failed to parse input"};
        }
    }

    namespace {
        template<typename T>
        std::ostream& operator<<(std::ostream& os, const std::optional<T>& r) noexcept {
            if (std::ostream::sentry s(os); s) {
                if (r) os << r.value();
                else   os << "N/A";
            }
            return os;
        }
    }

    template<>
    void solver<YEAR, DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << resultA(in);
    }

    template<>
    void solver<YEAR, DAY>::solveB(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << resultB(in);
    }

    template<>
    void* solver<YEAR, DAY>::parse(std::istream& is) const
    {
        std::istream::sentry s(is);
        if (!s) throw parse_exception{ "invalid input stream" };

        std::unique_ptr<input_t> ptr{ new input_t };
        input_t& in = *ptr;

        parse_input(is, in);

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        if constexpr (std::is_array_v<input_t>) delete[] reinterpret_cast<input_t*>(ptr);
        else delete reinterpret_cast<input_t*>(ptr);
    }
}