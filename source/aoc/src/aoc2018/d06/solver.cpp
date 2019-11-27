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
#include <functional>

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

        class sum_of_distance_curve {
        public:
            sum_of_distance_curve(const std::vector<intmax_t>& in, std::intmax_t max_dist) noexcept {
                std::vector<ivec2> curve;
                curve.reserve(in.size()+2);

                std::size_t npoints = in.size();

                // initialize ordinates as the minimum sum of distances
                auto mid = std::next(in.begin(), in.size() / 2);
                auto min_dist = std::accumulate(in.begin(), in.end(), std::intmax_t(0), [x = *mid](auto sum, auto p) { return sum + std::abs(p-x); });
                std::transform(in.begin(), in.end(), std::back_inserter(curve), [min_dist](auto x) { return ivec2{x, min_dist}; });
                
                // find mid points
                lmid_idx_ = (curve.size() - 1) / 2;
                rmid_idx_ = curve.size() / 2;

                // move origin to the middle region
                origin_ = (curve[lmid_idx_].x + curve[rmid_idx_].x) / 2;
                for (auto&[x, y] : curve) x -= origin_;

                // compute the sum of distances from mid-point to left
                for (std::size_t i = 0; i < lmid_idx_; ++i) {
                    std::size_t idx = lmid_idx_ - i - 1;
                    auto num_left = idx + 1;
                    auto num_right = curve.size() - num_left;
                    auto imbalance = (num_right - num_left);
                    curve[idx].y = curve[idx + 1].y + imbalance * (curve[idx+1].x - curve[idx].x);
                }

                // compute the sum of distances from mid-point to right
                for (std::size_t idx = rmid_idx_ + 1; idx < curve.size(); ++idx)
                {
                    auto num_left = idx;
                    auto num_right = curve.size() - num_left;
                    auto imbalance = (num_left - num_right);
                    curve[idx].y = curve[idx - 1].y + imbalance * (curve[idx].x - curve[idx - 1].x);
                }

                // delete repeated points
                curve.erase(std::unique(curve.begin(), curve.end()), curve.end());

                // extrapolate so that forward/reverse are always called with regular values
                if (std::intmax_t incr_y = (max_dist - curve.front().y); incr_y >= 0) {
                    std::intmax_t incr_x = 1 + incr_y / npoints;
                    std::intmax_t new_x = curve.front().x - incr_x;
                    std::intmax_t new_y = curve.front().y + npoints * (curve.front().x - new_x);
                    curve.insert(curve.begin(), ivec2{ new_x, new_y });
                }
                if (std::intmax_t incr_y = (max_dist - curve.back().y); incr_y >= 0) {
                    std::intmax_t incr_x = 1 + incr_y / npoints;
                    std::intmax_t new_x = curve.back().x + incr_x;
                    std::intmax_t new_y = curve.back().y + npoints * (new_x - curve.back().x);
                    curve.push_back(ivec2{ new_x, new_y });
                }

                // update mid indices
                lmid_idx_ = std::distance(curve.begin(), std::find_if(curve.begin(), curve.end(), [min_dist](const auto& xy) { return xy.y == min_dist; }));
                rmid_idx_ = lmid_idx_ + (npoints % 2 ? 1 : 0);

                // unpack x, y into different vectors
                abscissae_.reserve(curve.size());
                ordinates_.reserve(curve.size());
                std::transform(curve.begin(), curve.end(), std::back_inserter(abscissae_), [](const auto& xy) { return xy.x; });
                std::transform(curve.begin(), curve.end(), std::back_inserter(ordinates_), [](const auto& xy) { return xy.y; });
            }

            std::intmax_t forward(std::intmax_t x) const noexcept {
                x -= origin_;
                auto next = std::upper_bound(abscissae_.begin(), abscissae_.end(), x);
                assert(next != abscissae_.begin() && next != abscissae_.end());
                std::size_t idx = (std::size_t) std::distance(abscissae_.begin(), next);
                const auto& x0 = abscissae_[idx-1]; const auto& x1 = abscissae_[idx];
                const auto& y0 = ordinates_[idx-1]; const auto& y1 = ordinates_[idx];
                return y0 + (x - x0) * (y1 - y0) / (x1 - x0); // this division will be exact by construction
            }
            std::pair<std::intmax_t, std::intmax_t> reverse(std::intmax_t y) const noexcept {
                assert(y >= min_dist());
                std::intmax_t first, second;
                {
                    auto next = std::upper_bound(ordinates_.begin(), std::next(ordinates_.begin(), lmid_idx_), y, std::greater{});
                    std::size_t idx = (std::size_t) std::distance(ordinates_.begin(), next);
                    const auto& x0 = abscissae_[idx-1]; const auto& x1 = abscissae_[idx];
                    const auto& y0 = ordinates_[idx-1]; const auto& y1 = ordinates_[idx];
                    // round towards x1
                    first = origin_ + x1 - (y1 - y) * (x1 - x0) / (y1 - y0); 
                }
                {
                    auto next = std::upper_bound(std::next(ordinates_.begin(), rmid_idx_), std::prev(ordinates_.end()), y);
                    std::size_t idx = (std::size_t) std::distance(ordinates_.begin(), next);
                    auto x0 = abscissae_[idx-1]; auto x1 = abscissae_[idx];
                    auto y0 = ordinates_[idx-1]; auto y1 = ordinates_[idx];
                    // round towards x0
                    second = origin_ + x0 + (y - y0) * (x1 - x0) / (y1 - y0); 
                }

                return { first, second };
            }

            std::intmax_t min_dist() const noexcept {
                return ordinates_[lmid_idx_];
            }

        private:
            std::vector<intmax_t> abscissae_;
            std::vector<intmax_t> ordinates_;
            std::size_t lmid_idx_, rmid_idx_;
            std::intmax_t origin_;
        };

        std::size_t resultB(const input_t& in, std::size_t max_dist = 9999) {
            // This problem can be thought in terms of the gradient of the sum of the distances.
            // In particular, given points P = { in[0], ..., in[N-1] }, there is a squared
            // region that leaves half the points above and half the points below, and
            // half the points to the left and half to the right.
            // This is the region sum of distances.
            // Considering just the x coordinate, for each movement outside
            // this inner region, the total sum of distances increases by the imbalance
            // in points that are to the left w.r.t. the points to the right.
            //
            // For example, imagine points whose x are 0, 1, 5, 8.
            // All points with x between 1 and 5 (inclusive) have all the same sum of distances
            // of 12.
            // The point x = 0 will have 12 + 2 = 14, as the imbalance was of 1 point more to the right.
            // Point x = -1 will have 14 + 4 = 18, as the imabalance was of 4 points more to the right.
            // From then on, for each movement to the left the total sum of distances increases by 4.
            std::vector<std::intmax_t> x_values, y_values;
            x_values.reserve(in.size());
            y_values.reserve(in.size());
            std::transform(in.begin(), in.end(), std::back_inserter(x_values), [](const auto& p) { return p.x; });
            std::transform(in.begin(), in.end(), std::back_inserter(y_values), [](const auto& p) { return p.y; });
            std::sort(x_values.begin(), x_values.end());
            std::sort(y_values.begin(), y_values.end());

            // Find sum of distances curve.
            sum_of_distance_curve x_curve{ x_values, std::intmax_t(max_dist) };
            sum_of_distance_curve y_curve{ y_values, std::intmax_t(max_dist) };

            if (std::size_t(x_curve.min_dist() + y_curve.min_dist()) > max_dist) return 0;

            std::intmax_t accum = 0;
            auto [x0, x1] = x_curve.reverse(max_dist - y_curve.min_dist());
            for (auto x = x0; x <= x1; ++x) {
                auto [y0, y1] = y_curve.reverse(max_dist - x_curve.forward(x));
                accum += y1 - y0 + 1;
            }
            return std::size_t(accum);
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
        os << resultB(in);
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