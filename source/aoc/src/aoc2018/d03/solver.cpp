#include "aoc/solver.h"
#include "aoc/error.h"

#include <ddr/math/vector.h>
#include <ddr/math/grid.h>
#include <ddr/math/hyperbox.h>

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

#include "version.h"

namespace aoc {
    namespace {

        using vec2 = ddr::math::ivec2;

        using rect_t = std::tuple<std::size_t, ddr::math::rect>;

        using input_t = std::vector<rect_t>;

        template<typename T> using board_t = ddr::math::grid2<T>;

        std::size_t resultA(const input_t& in) noexcept
        {
            board_t<std::size_t> board;

            // resize board to hold all rects
            {
                vec2 max;
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
                vec2 max;
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
    void solver<AOC_YEAR, AOC_DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << resultA(in);
    }

    template<>
    void solver<AOC_YEAR, AOC_DAY>::solveB(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        auto result = resultB(in);
        if (result) os << *result;
        else os << "N/A";
    }

    template<>
    void* solver<AOC_YEAR, AOC_DAY>::parse(std::istream& is) const
    {
        if (!is) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };

        input_t& vec = *ptr;

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
            auto&[id, r] = vec.emplace_back();
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

        return ptr.release();
    }

    template<>
    void solver<AOC_YEAR, AOC_DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}