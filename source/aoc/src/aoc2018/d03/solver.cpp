#include "aoc/solver.h"

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>
#include <memory>
#include <sstream>
#include <array>
#include <string_view>
#include <string>
#include <cstring>
#include <type_traits>
#include <cstdlib>
#include <tuple>
#include <regex>

#include <ddr/math/vector.h>
#include <ddr/math/grid.h>
#include <ddr/math/hyperbox.h>

namespace aoc {
namespace {
    constexpr std::size_t YEAR = 2018;
    constexpr std::size_t  DAY = 3;

    using ivec2 = ddr::math::ivec2;
    using rect_t = std::tuple<std::size_t, ddr::math::rect>;
    using input_t = std::vector<rect_t>;
    template<typename T> using board_t = ddr::math::grid2<T>;

    input_t parse_input(std::istream& is)
    {
        input_t in;

        std::regex re{ R"re(#(\d+) @ (\d+),(\d+): (\d+)x(\d+))re" };
        std::string line;
        while (std::getline(is, line))
        {
            if (line.empty()) continue;
            if (std::smatch m; std::regex_match(line, m, re)) {
                auto&[id, r] = in.emplace_back();
                id = std::stoi(m[1].str());
                r.start.x = std::stoi(m[2].str());
                r.start.y = std::stoi(m[3].str());
                r.size.x = std::stoi(m[4].str());
                r.size.y = std::stoi(m[5].str());
            } else throw parse_exception{"unexpected line format"};
        }

        return in;
    }

    std::size_t resultA(const input_t& in) noexcept
    {
        board_t<std::size_t> board;

        // resize board to hold all rects
        {
            ivec2 max;
            for (const auto&[id, r] : in) {
                max.x = std::max(max.x, r.start.x + r.size.x);
                max.y = std::max(max.y, r.start.y + r.size.y);
            }
            board.resize(max);
        }

        for (const auto&[id, r] : in)
            for (const auto& idx : r)
                ++board[idx];

        return std::count_if(board.begin(), board.end(), [](auto n) { return n > 1; });
    }

    std::optional<std::size_t> resultB(const input_t& in) noexcept
    {
        board_t<std::size_t> board;

        // resize board to hold all rects
        {
            ivec2 max;
            for (const auto& [id, r] : in) {
                max.x = std::max(max.x, r.start.x + r.size.x);
                max.y = std::max(max.y, r.start.y + r.size.y);
            }
            board.resize(max);
        }

        for (const auto& [id, r] : in)
            for (const auto& idx : r)
                ++board[idx];

        for (const auto& [id, r] : in) {
            bool found = true;
            for (const auto& idx : r) {
                if (board[idx] != 1) {
                    found = false;
                    break;
                }
            }
            if (found) return id;
        }
        return std::nullopt;
    }
}

    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}