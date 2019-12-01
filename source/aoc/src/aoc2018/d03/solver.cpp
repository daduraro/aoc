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

            auto expect = [](std::istream& is, char c)
            {
                char e;
                if (!(is >> e)) throw parse_exception{};
                if (e != c)  throw parse_exception{};
            };

            std::string line;
            while (std::getline(is, line))
            {
                if (line.empty()) continue;
                std::stringstream line_ss{ std::move(line) };
                auto&[id, r] = in.emplace_back();
                expect(line_ss, '#');
                if (!(line_ss >> id)) throw parse_exception{};
                expect(line_ss, '@');
                if (!(line_ss >> r.start.x)) throw parse_exception{};
                expect(line_ss, ',');
                if (!(line_ss >> r.start.y)) throw parse_exception{};
                expect(line_ss, ':');
                if (!(line_ss >> r.size.x)) throw parse_exception{};
                expect(line_ss, 'x');
                if (!(line_ss >> r.size.y).eof()) throw parse_exception{};

                if (r.start.x < 0 || r.start.y < 0 || r.size.x < 0 || r.size.y < 0)
                    throw parse_exception{};
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