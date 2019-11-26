#include "aoc/solver.h"
#include "aoc/error.h"

#include <cassert>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>

#include <ddr/math/vector.h>
#include <ddr/math/grid.h>
#include <ddr/math/hyperbox.h>

#include "version.h"

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = AOC_YEAR;
        constexpr std::size_t DAY = AOC_DAY;
        using ivec2 = ddr::math::ivec2;
        using input_t = std::vector<ivec2>;

        std::optional<std::size_t> resultA(input_t in) {
            // We need to compute the Voronoi diagram using Manhattan distance.
            // As our space is discrete, we can brute-force it.
            using namespace ddr::math;

            // Move to a more appropiate origin.
            auto min_point = std::reduce(std::next(in.begin()), in.end(), *in.begin(), [](const auto& lhs, const auto& rhs) { return min(lhs, rhs); });
            std::transform(in.begin(), in.end(), in.begin(), [min_point](const auto& x){ return x - min_point; });
            auto max_point = std::reduce(std::next(in.begin()), in.end(), *in.begin(), [](const auto& lhs, const auto& rhs) { return max(lhs, rhs); });
            auto size_board = max_point + ivec2{1,1};
            
            // Bounding box now is { (0,0), max_point }.
            grid2<std::optional<std::size_t>> closest_point{ size_board };

            for (const auto& p : rect{ ivec2{0,0}, size_board }) {
                auto min_i = std::numeric_limits<std::size_t>::max();
                auto min_dist = std::numeric_limits<std::intmax_t>::max();
                bool more_than_once = false;
                for (std::size_t i = 0; i < in.size(); ++i) {
                    const auto& x = in[i];
                    auto d = manh(x, p);
                    if (d == min_dist) more_than_once = true;
                    else if (d < min_dist) {
                        more_than_once = false;
                        min_dist = d;
                        min_i = i;
                    }
                }
                closest_point[p] = more_than_once ? std::nullopt : std::optional{ min_i };
            }

            std::vector<std::optional<std::size_t>> region_size;
            region_size.resize(in.size(), 0);

            // Compute the interior regions, assuming all region_size are valid optionals.
            for (auto p : rect{ivec2{1, 1}, size_board - ivec2{2,2}}) {
                if (auto idx = closest_point[p]; idx)
                    ++(*region_size[*idx]);
            }

            // Any point whose region include a point in the boundary will
            // have an infinite region, discard those.
            for (std::intmax_t x = 0; x < size_board.x; ++x) {
                auto i = closest_point[{x, 0}];
                if (i) region_size[*i] = std::nullopt;
                auto j = closest_point[{x, max_point.y}];
                if (j) region_size[*j] = std::nullopt;
            }
            for (std::intmax_t y = 0; y < size_board.y; ++y) {
                auto i = closest_point[{0, y}];
                if (i) region_size[*i] = std::nullopt;
                auto j = closest_point[{max_point.x, y}];
                if (j) region_size[*j] = std::nullopt;
            }

            std::size_t result = 0;
            for (const auto& sz : region_size) {
                if (sz) result = std::max(result, *sz);
            }

            return (result == 0) ? std::nullopt : std::optional{result};
        }

        std::size_t resultB(input_t in, std::size_t max_dist = 10000) {
        //     // This problem can be thought in terms of the gradient of the sum of the distances.
        //     // In particular, given points P = { in[0], ..., in[N-1] }, there is a squared
        //     // region that leaves half the points above and half the points below, and
        //     // half the points to the left and half to the right. 
        //     // This is the region sum of distances.
        //     // Considering just the x coordinate, for each movement outside
        //     // this inner region, the total sum of distances increases by the imbalance
        //     // in points that are to the left w.r.t. the points to the right.
        //     //
        //     // For example, imagine points whose x are 0, 1, 5, 8.
        //     // All points with x between 1 and 5 (inclusive) have all the same sum of distances
        //     // of 12. 
        //     // The point x = 0 will have 12 + 2 = 14, as the imbalance was of 1 point more to the right.
        //     // Point x = -1 will have 14 + 4 = 18, as the imabalance was of 4 points more to the right.
        //     // From then on, for each movement to the left the total sum of distances increases by 4.
            
        //     std::vector<std::intmax_t> x_values, y_values;
        //     x_values.reserve(in.size());
        //     y_values.reserve(in.size());
        //     std::transform(in.begin(), in.end(), std::back_inserter(x_values), [](const auto& p) { return p.x; });
        //     std::transform(in.begin(), in.end(), std::back_inserter(y_values), [](const auto& p) { return p.y; });
        //     std::sort(x_values.begin(), x_values.end());
        //     std::sort(y_values.begin(), y_values.end());

        //     // Find the minimum distance.
        //     auto left = std::next(x_values.begin(), x_values.size() / 2);
        //     auto bot = std::next(y_values.begin(), y_values.size() / 2);

        //     auto total_dist = [](const std::vector<std::intmax_t>& vs, std::intmax_t x) -> std::intmax_t {
        //         return std::accumulate(vs.begin(), vs.end(), std::intmax_t(0), [x](std::intmax_t accum, std::intmax_t y) { return accum + std::abs(x-y); });
        //     };

        //     auto smallest_dist = total_dist(x_values, *left) + total_dist(y_values, *bot);
        //     if (smallest_dist > max_dist) return 0;

        //     // Solve the problem on R (as the solution is separable).
        //     auto size_below_dist_factory = [allowance = max_dist - smallest_dist](const std::vector<std::intmax_t>& pts) {
        //         auto left = std::next(pts.rbegin(), (pts.size() + 1) / 2);
        //         auto right = std::next(pts.begin(), (pts.size() + 1) / 2);

        //         std::vector<std::intmax_t> left_dist, right_dist;
        //         auto compute_dist_vector = [dist_ = allowance](auto start, auto end, auto pos_type) {
        //             std::vector<std::intmax_t> result;
        //             auto dist = dist_;
        //             constexpr bool pos = decltype(pos_type)::value;
        //             auto it = start;
        //             for (int x = *start; dist >= 0; x += (pos ? 1 : -1)) {
        //                 while (it != end && (pos ? *it > x : *it < x)) ++it;
        //                 dist -= std::distance(start, it);
        //                 result.push_back(dist);
        //             }
        //             return result;
        //         };

        //         auto left_dist = compute_dist_vector(left, pts.rend(), std::false_type{});
        //         auto right_dist = compute_dist_vector(right, pts.end(), std::true_type{});


        //         return [left_dist = std::move(left_dist), right_dist = std::move(right_dist), left_v = *left, right_v = *right]
        //         (std::intmax_t x) -> std::intmax_t
        //         {
        //             if (left_v <= x && x <= right_v) return 0;

        //         };
        //     };
            return 0;
        }

    }

    template<>
    void solver<YEAR, DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        auto result = resultA(in);
        if (result) os << *result;
        else os << "N/A";
    }

    template<>
    void solver<YEAR, DAY>::solveB(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << "N/A";
    }

    template<>
    void* solver<YEAR, DAY>::parse(std::istream& is) const
    {
        std::istream::sentry s(is);
        if (!s) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };
        input_t& vec = *ptr;

        auto expect = [](std::istream& is, char c)
        {
            char e;
            if ((is >> e).fail()) throw parse_exception{};
            if (e != c)  throw parse_exception{};
        };

        auto parse = [](std::istream& is, auto& x)
        {
            if ((is >> x).fail()) throw parse_exception{};
        };

        for (std::string line; std::getline(is, line); ) {
            if (line.empty()) continue;
            std::stringstream ss{ line };
            auto&[x, y] = vec.emplace_back();
            parse(ss, x);
            expect(ss, ',');
            parse(ss, y);
        }

        if (vec.empty()) throw parse_exception{};

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}