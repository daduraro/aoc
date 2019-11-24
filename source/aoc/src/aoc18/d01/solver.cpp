#include "aoc/solver.h"
#include "aoc/error.h"

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

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = 2018;
        constexpr std::size_t DAY = 01;
        using input_t = std::vector<std::intmax_t>;

        std::optional<intmax_t> first_repetition(const std::vector<std::intmax_t>& fs) noexcept
        {
            // can be made more efficient (n log n)
            for (std::size_t i = 0; i+1 < fs.size(); ++i) {
                for (std::size_t j = i+1; j < fs.size(); ++j)
                {
                    if (fs[i] == fs[j]) return fs[i];
                }
            }
            return std::nullopt;
        }

        std::optional<intmax_t> result_B(const input_t& input) noexcept
        {
            // given frequencies f_i = input[0] + ... + input[i], i < N-1
            // we are looking for:
            //  given d = input[0] + ... + input[N]
            //  if d == 0
            //    f_j s.t. argmin(i) f_i = f_j,  i!=j (if exists)
            //    f_0 (otherwise)
            //  else
            //    f_j s.t. argmin(k in [0,inf); i) f_i + k*d = f_j,  j != i
            //
            // This is derived from the following observations:
            //   - Consider f[i] the frequency after adding input[0 % N] + ... + input[(i-1) % N], with f[0] = 0,
            //     where N is the total number of input.
            //   - It is easy to see that given d = input[0] + ... + input[N-1],
            //     all the frequencies that will be visited are, in order (read first left-to-right then top-to-bottom)
            //       f[0]     ,  f[1]     ,  f[2]     ,  ... ,  f[N-1]     ,
            //       f[0] +  d,  f[1] +  d,  f[2] +  d,  ... ,  f[N-1] +  d,
            //       f[0] + 2d,  f[1] + 2d,  f[2] + 2d,  ... ,  f[N-1] + 2d,
            //       f[0] + 3d,  f[1] + 3d,  f[2] + 3d,  ... ,  f[N-1] + 3d,
            //       f[0] + 4d,  f[1] + 4d,  f[2] + 4d,  ... ,  f[N-1] + 4d,
            //       ...
            //   - It is also easy to see that if f[i] + nd = f[j] + md, n > m, then f[i] + (n-m)d = f[j],
            //     thus the first repeated frequency will be one of the f[0], ..., f[N-1].
            // Considering now the case where d != 0, a frequency f[j], 0 <= j < N, will be repeated if there
            // is a f[i], i != j (because d!=0), such that f[i] + kd = f[j], k >= 0. The first repetition will
            // occur, then, when k is the smallest possible, and, in case of multiple i, j such that
            // f[i] + kd = f[j], then the first one will be the one such that i is smaller, as f[i_0] + kd, will occur
            // before f[i_1] + kd, if i_0 < i_1.
            //
            // The case where d == 0 is simpler in the sense that f[0] is exactly f[N], thus we should only
            // check if there is any f[i] = f[j], i < j, and the result will be the one with the smallest i.
            using std::begin, std::end;

            std::intmax_t d = std::accumulate(begin(input), end(input), (std::intmax_t)0);

            std::vector<std::intmax_t> fs;
            fs.reserve(input.size());
            fs.push_back(0);

            std::transform(begin(input), std::prev(end(input)), std::back_inserter(fs), [s = std::intmax_t(0)](auto i) mutable {
                return (s+=i);
            });

            assert(!fs.empty());
            if (d == 0) {
                return first_repetition(fs).value_or(fs[0]);
            }

            std::intmax_t result = 0;
            std::uintmax_t not_found = std::numeric_limits<std::uintmax_t>::max();
            std::uintmax_t k = not_found;
            for (std::size_t i = 0; i < fs.size(); ++i) {
                for (std::size_t j = 0; j < fs.size(); ++j) {
                    // fs[i] + k*d = fs[j]
                    // k*d = fs[j] - fs[i]
                    auto diff = fs[j] - fs[i];
                    auto remquo = std::div(diff, d);
                    // help branch predictor using & instead of &&
                    // the condition checks the following:
                    //   if i is equal to j, then we should skip this iteration
                    //   only when the remainder is 0 f[j] can be reached from f[i]
                    //   we must ensure k >= 0 (k=quotient)
                    //   finally we should only get the value if obtained k is less than previously obtained k, sorting by i is done implicitly
                    if ((i != j) & (remquo.rem == 0) & (remquo.quot >= 0) & (std::uintmax_t(remquo.quot) < k)) {
                        result = fs[j];
                        k = remquo.quot;
                    }
                }
            }
            return k != not_found ? std::optional(result) : std::nullopt;
        }

    }

    template<>
    void solver<YEAR, DAY>::solveA(std::ostream& os, const void* in) const
    {
        const input_t& input = *reinterpret_cast<const input_t*>(in);
        os << std::accumulate(std::begin(input), std::end(input), (std::intmax_t)0);
    }

    template<>
    void solver<YEAR, DAY>::solveB(std::ostream& os, const void* in) const
    {
        const input_t& input = *reinterpret_cast<const input_t*>(in);
        auto res = result_B(input);
        if (res) os << *res;
        else os << "N/A";
    }

    template<>
    void* solver<YEAR, DAY>::parse(std::istream& is) const
    {
        std::unique_ptr<input_t> ptr{ new input_t{} };

        input_t& vec = *ptr;
        using is_it_t = std::istream_iterator<std::intmax_t>;
        std::copy(is_it_t(is), is_it_t(), std::back_inserter(vec));
        if (vec.empty()) throw parse_exception{};

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}