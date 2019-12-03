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
#include <utility>
#include <sstream>

#include <ddr/math/vector.h>

namespace aoc {
namespace {
    constexpr std::size_t YEAR = 2019;
    constexpr std::size_t DAY  = 3;

    enum class direction_t {
        up,
        down,
        left,
        right
    };
    using ivec2 = ddr::math::ivec2;
    using move_t = std::tuple<direction_t, std::uintmax_t>;
    using curve_t = std::vector<move_t>;
    using input_t = std::array<curve_t, 2>;

    input_t parse_input(std::istream& is)
    {
        input_t in;
        std::string line, part;
        std::size_t i = 0;
        while (std::getline(is, line)) {
            if (line.empty()) continue;
            if (i >= 2) throw parse_exception{};
            auto& curve = in[i++];
            std::stringstream ss{line};
            while (std::getline(ss, part, ',')) {
                std::stringstream substr{part};
                char dir;
                std::uintmax_t value;
                if (!(substr >> dir >> value)) throw parse_exception{};
                direction_t d;
                switch (dir) {
                    default: throw parse_exception{};
                    case 'U': d = direction_t::up; break;
                    case 'R': d = direction_t::right; break;
                    case 'D': d = direction_t::down; break;
                    case 'L': d = direction_t::left; break;
                }
                curve.emplace_back(d, value);
            }
        }
        return in;
    }

    auto resultA(const input_t& in) noexcept -> std::optional<std::size_t>
    {
        using bounds_t = std::array<std::intmax_t, 2>; // first value should always be smaller than the second
        using straight_t = std::pair<std::intmax_t, bounds_t>;

        enum class straight_type_t {
            vertical,
            horizontal
        };

        auto get_straight = [](ivec2& offset, const move_t& move) -> std::tuple<straight_type_t, straight_t>
        {
            auto[d, m] = move;
            switch (d) {
                default: assert(false);
                case direction_t::up:    offset += ivec2{  0,  m }; return { straight_type_t::vertical,   { offset.x, { offset.y - m, offset.y } } };
                case direction_t::down:  offset += ivec2{  0, -m }; return { straight_type_t::vertical,   { offset.x, { offset.y, offset.y + m } } };
                case direction_t::left:  offset += ivec2{ -m,  0 }; return { straight_type_t::horizontal, { offset.y, { offset.x, offset.x + m } } };
                case direction_t::right: offset += ivec2{  m,  0 }; return { straight_type_t::horizontal, { offset.y, { offset.x - m, offset.x } } };
            }
        };

        // divide first curve into vertical and horizontal segments
        std::vector<straight_t> vsegments; vsegments.reserve(in[0].size() / 2);
        std::vector<straight_t> hsegments; hsegments.reserve(in[0].size() / 2);
        {
            ivec2 pos = { 0, 0 };
            for (const auto& m : in[0]) {
                auto[t, s] = get_straight(pos, m);
                switch (t) {
                    default: assert(false);
                    case straight_type_t::horizontal: hsegments.emplace_back(std::move(s)); break;
                    case straight_type_t::vertical:   vsegments.emplace_back(std::move(s)); break;
                }
            }
            std::sort(vsegments.begin(), vsegments.end(), [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < std::get<0>(rhs); });
            std::sort(hsegments.begin(), hsegments.end(), [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < std::get<0>(rhs); });
        }

        auto min_cross_intersections = [](const straight_t& s, const std::vector<straight_t>& lines) -> std::optional<std::size_t>
        {
            std::optional<std::size_t> dist;

            const auto&[x, ys] = s;
            const auto&[y0, y1] = ys;

            auto it   = std::lower_bound(lines.begin(), lines.end(), y0, [](const auto& elem, const auto& value) { return std::get<0>(elem) < value; } );
            auto last = std::upper_bound(lines.begin(), lines.end(), y1, [](const auto& elem, const auto& value) { return std::get<0>(elem) < value; } );

            for (; it != last; ++it) {
                if (x >= it->second[0] && x <= it->second[1]) {
                    // intersection occur
                    std::size_t d = std::size_t(std::abs(it->first) + std::abs(x));
                    if (d != 0)
                        dist = std::min(dist.value_or(std::numeric_limits<std::size_t>::max()), d);
                }
            }
            return dist;
        };

        auto min_parallel_intersections = [](const straight_t& s, const std::vector<straight_t>& lines) -> std::optional<std::size_t>
        {
            std::optional<std::size_t> dist;

            const auto&[x, ys] = s;
            const auto&[y0, y1] = ys;

            auto it   = std::lower_bound(lines.begin(), lines.end(), y0, [](const auto& elem, const auto& value) { return std::get<0>(elem) < value; } );
            auto last = std::upper_bound(lines.begin(), lines.end(), y1, [](const auto& elem, const auto& value) { return std::get<0>(elem) < value; } );

            for (; it != last; ++it) {
                if (x >= it->second[0] && x <= it->second[1]) {
                    // intersection occur
                    std::size_t d = std::size_t(std::abs(it->first) + std::abs(x));
                    if (d != 0)
                        dist = std::min(dist.value_or(std::numeric_limits<std::size_t>::max()), d);
                }
            }
            return dist;
        };

        return std::nullopt;
    }

    auto resultB(const input_t& in) noexcept -> std::optional<std::size_t>
    {
        // ...
        return std::nullopt;
    }
}

    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}