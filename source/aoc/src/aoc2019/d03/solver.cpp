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
#include <tuple>
#include <utility>

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
                if (value > 0) curve.emplace_back(d, value);
            }
        }
        return in;
    }

    using straight_t = std::tuple<ivec2, ivec2, std::size_t>; // pos, vec, pos_total_move

    auto to_vec(const move_t& move) -> ivec2
    {
        auto[d, m_unsigned] = move;
        auto m = std::intmax_t(m_unsigned);
        switch (d) {
            default: assert(false);
            case direction_t::up:    return ivec2{  0,  m };
            case direction_t::down:  return ivec2{  0, -m };
            case direction_t::left:  return ivec2{ -m,  0 };
            case direction_t::right: return ivec2{  m,  0 };
        }
    }

    template<bool partA>
    auto min_cross_intersections(std::bool_constant<partA>, const straight_t& s, const std::vector<straight_t>& segments) noexcept -> std::optional<std::size_t> 
    {
        const auto&[p0, v0, s0] = s;

        // assume x coordinate where s projects into a line
        std::size_t idx_x = (v0.x == 0) ? 0 : 1;
        std::size_t idx_y = (v0.x == 0) ? 1 : 0;

        // obtain relevant coordinates
        auto x = p0[idx_x];
        auto y0 = p0[idx_y];
        auto y1 = p0[idx_y] + v0[idx_y];

        auto y_min = std::min(y0, y1);
        auto y_max = std::max(y0, y1);

        // get candidates to intersect
        auto it = std::lower_bound(segments.begin(), segments.end(), y_min, [idx_y](const auto& elem, const auto& value) {
            return std::get<0>(elem)[idx_y] < value;
        });
        auto last = std::upper_bound(segments.begin(), segments.end(), y_max, [idx_y](const auto& value, const auto& elem) {
            return value < std::get<0>(elem)[idx_y];
        });

        // check each intersection point
        std::optional<std::size_t> dist;
        for (; it != last; ++it) {
            const auto&[p1, v1, s1] = *it;

            auto y = p1[idx_y];
            auto x0 = p1[idx_x];
            auto x1 = p1[idx_x] + v1[idx_x];

            auto x_min = std::min(x0, x1);
            auto x_max = std::max(x0, x1);

            if (x >= x_min && x <= x_max) {
                // intersection point has coordinates x and y
                std::size_t d;
                if constexpr (partA) {
                    d = std::size_t(std::abs(x) + std::abs(y));
                } else {
                    d = s0 + s1;
                    ivec2 intersection_point;
                    intersection_point[idx_x] = x;
                    intersection_point[idx_y] = y;

                    d += manh(intersection_point, p0);
                    d += manh(intersection_point, p1);
                }
                if (d != 0)
                    dist = dist ? std::min(*dist, d) : d;
            }
        }

        return dist;
    }


    // TODO: current implementation only handles crossing intersections and do not consider intersections
    //       where both segments are coincident
    // template<bool partA>
    // auto min_parallel_intersections(std::bool_constant<partA>, const straight_t& s, const std::vector<straight_t>& lines) -> std::optional<std::size_t>
    // {
    //     const auto&[p0, v0, s0] = s;

    //     // assume x the coordinate that does not vary
    //     std::size_t idx_x = (v0[0] == 0) ? 0 : 1;
    //     std::size_t idx_y = (v0[0] == 0) ? 1 : 0;

    //     // obtain relevant coordinates
    //     auto x = p0[idx_x];
    //     auto c0_y0 = p0[idx_y];
    //     auto c0_y1 = p0[idx_y] + v0[idx_y];

    //     auto c0_ymin = std::min(c0_y0, c0_y1);
    //     auto c0_ymax = std::max(c0_y0, c0_y1);

    //     // do equal range on x
    //     auto it   = std::lower_bound(lines.begin(), lines.end(), x, [x_idx](const auto& elem, const auto& value) { 
    //         return std::get<0>(elem)[x_idx] < value; 
    //     });
    //     auto last = std::upper_bound(lines.begin(), lines.end(), x, [x_idx](const auto& value, const auto& elem) { 
    //         return value < std::get<0>(elem)[x_idx]; 
    //     });

    //     std::optional<std::size_t> dist;
    //     for (; it != last; ++it) {
    //         const auto&[p1, v1, s1] = *it;

    //         auto c1_y0 = p1[idx_y];
    //         auto c1_y1 = p1[idx_y] + v1[idx_y];

    //         auto c1_ymin = std::min(c1_y0, c1_y1);
    //         auto c1_ymax = std::max(c1_y0, c1_y1);


    //         if (c0_ymin <= c1_ymax && c1_ymin <= c0_ymax) {
    //             // intersection occur, find all the range of intersection
    //             auto y_inter_min = std::max(c0_ymin, c1_ymin);
    //             auto y_inter_max = std::min(c0_ymax, c1_ymax);

    //             if constexpr (partA) { // manhattan distance
    //                 // two cases, y_inter_min * y_inter_max > 0:
    //                 //  then the value of intersection is either y_inter_min or y_inter_max
    //                 // y_inter_min * y_inter_max <= 0:
    //                 //  then the value is 0 if x != 0, otherwise sign(y_inter_min) or sign(y_inter_max) (if different than 0)
    //                 if (y_inter_min * y_inter_max > 0) {
    //                     auto d = std::size_t(std::abs(x) + std::min(std::abs(y_inter_min), std::abs(y_inter_max)));
    //                     dist = dist ? std::min(*dist, d) : d;
    //                 } else if (x == 0) {
    //                     if (y_inter_min != 0) y_inter_min /= std::abs(y_inter_min);
    //                     if (y_inter_max != 0) y_inter_max /= std::abs(y_inter_max);

    //                     auto d = std::size_t(std::max(std::abs(y_inter_min), std::abs(y_inter_max)));
    //                     if (d != 0) dist = dist ? std::min(*dist, d) : d;
    //                 } else {
    //                     auto d = std::size_t(std::abs(x));
    //                     dist = dist ? std::min(*dist, d) : d;
    //                 }
    //             } else {

    //             }
    //         }
    //     }
    //     return dist;
    // }

    template<bool partA>
    auto result(const input_t& in) noexcept -> std::optional<std::size_t>
    {
        // divide first curve into vertical and horizontal segments
        std::vector<straight_t> vsegments; vsegments.reserve(in[0].size() / 2);
        std::vector<straight_t> hsegments; hsegments.reserve(in[0].size() / 2);
        {
            std::size_t total_dist = 0;
            ivec2 pos = { 0, 0 };
            for (const auto& m : in[0]) {
                auto vec = to_vec(m);
                if (vec.x != 0) hsegments.emplace_back(pos, vec, total_dist);
                else vsegments.emplace_back(pos, vec, total_dist);

                pos += vec;
                total_dist += std::abs(vec.x) + std::abs(vec.y);
            }
            std::sort(vsegments.begin(), vsegments.end(), [](const straight_t& lhs, const straight_t& rhs) { return std::get<0>(lhs).x < std::get<0>(rhs).x; });
            std::sort(hsegments.begin(), hsegments.end(), [](const straight_t& lhs, const straight_t& rhs) { return std::get<0>(lhs).y < std::get<0>(rhs).y; });
        }

        std::optional<std::size_t> dist;
        ivec2 pos = { 0, 0 };
        std::size_t total_dist = 0;
        for (const auto& m : in[1]) {
            auto vec = to_vec(m);
            straight_t s{pos, vec, total_dist};
            const auto* cross = (vec.x == 0) ? &hsegments : &vsegments;
            if (auto d = min_cross_intersections(std::bool_constant<partA>{}, s, *cross); d)
                dist = dist ? std::min(*dist, *d) : *d;
            pos += vec;
            total_dist += std::abs(vec.x) + std::abs(vec.y);
        }
        return dist;
    }
}

    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, result<true>, result<false>);
    }
}