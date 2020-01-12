#include "aoc/solver.h"

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

namespace {
    using namespace aoc;
    constexpr std::size_t YEAR = 2019;
    constexpr std::size_t DAY  = 1;
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

    input_t parse_input(std::istream& is)
    {
        input_t in;
        using input_it = std::istream_iterator<std::size_t>;
        std::copy(input_it(is), input_it(), std::back_inserter(in));
        if (!is.eof()) throw parse_exception{"failed to parse input"};

        return in;
    }
}

namespace aoc {
    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}