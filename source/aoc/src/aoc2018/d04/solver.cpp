#include "aoc/solver.h"
#include "aoc/error.h"
#include "version.h"

#include "ddr/porting.h"

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

namespace aoc
{
    namespace {
        class date_t {
        public:
            // regular fake dates, where months have 31 days, and a year always have 12 months
            date_t() noexcept = default;
            constexpr date_t(std::intmax_t mins_since_epoch) noexcept : mins_since_epoch_(mins_since_epoch) {};
            constexpr date_t(std::intmax_t years, std::intmax_t months, std::intmax_t days, std::intmax_t hours, std::intmax_t minutes) noexcept
                : mins_since_epoch_(minutes + 60 * (hours + 24 * (days + 31 * (months + 12 * years)))) {};
            constexpr std::intmax_t since_epoch() const noexcept { return mins_since_epoch_; }
            constexpr unsigned min() const noexcept { return unsigned(((mins_since_epoch_ % 60) + 60) % 60); }

            friend constexpr bool operator==(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch_ == rhs.mins_since_epoch_; }
            friend constexpr bool operator!=(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch_ != rhs.mins_since_epoch_; }
            friend constexpr bool operator<(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch_ < rhs.mins_since_epoch_; }
            friend constexpr bool operator<=(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch_ <= rhs.mins_since_epoch_; }
            friend constexpr bool operator>(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch_ > rhs.mins_since_epoch_; }
            friend constexpr bool operator>=(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch_ >= rhs.mins_since_epoch_; }

        private:
            std::intmax_t mins_since_epoch_;
        };

        enum class event_t {
            begin,
            sleep,
            awake
        };

        using id_t = std::size_t;
        using record_t = std::tuple<date_t, id_t, event_t>;
        using input_t = std::vector<record_t>;

        template<bool partA>
        std::size_t result(input_t in, std::bool_constant<partA>) noexcept
        {
            // sort by guard id, preserving time order
            std::stable_sort(in.begin(), in.end(), [](const auto& lhs, const auto& rhs) { return std::get<id_t>(lhs) < std::get<id_t>(rhs); });

            using guard_info_t = std::tuple<id_t, std::size_t, std::size_t, std::size_t>;
            std::vector<guard_info_t> guards;
            for (auto first = in.begin(); first != in.end();)
            {
                auto last = std::next(std::adjacent_find(first, std::prev(in.end()), [](const auto& lhs, const auto& rhs){ return std::get<id_t>(lhs) != std::get<id_t>(rhs); }));

                auto&[id, total, best_minute, total_best_minute] = guards.emplace_back();

                id = std::get<id_t>(*first);

                std::array<std::size_t, 60> totals{};
                std::optional<date_t> sleeping_since;
                for (auto event_it = first; event_it != last; ++event_it) {
                    switch (std::get<event_t>(*event_it)) {
                    default: assert(false);
                    case event_t::begin: break;
                    case event_t::sleep: sleeping_since = sleeping_since.value_or(std::get<date_t>(*event_it)); break;
                    case event_t::awake:
                        if (sleeping_since) {
                            const date_t& t = std::get<date_t>(*event_it);
                            assert(t.since_epoch() - sleeping_since->since_epoch() < 60);
                            for (auto i = sleeping_since->since_epoch(); i < t.since_epoch(); ++i) {
                                ++totals[i%60];
                            }
                            sleeping_since.reset();
                        }
                        break;
                    }
                }

                total = std::accumulate(totals.begin(), totals.end(), (std::size_t)0);
                auto max_bm = std::max_element(totals.begin(), totals.end());
                best_minute = (std::size_t) std::distance(totals.begin(), max_bm);
                total_best_minute = *max_bm;

                std::swap(first, last);
            }

            auto max_it = std::max_element(guards.begin(), guards.end(), [](const auto& x, const auto& y){ return std::get<partA ? 1 : 3>(x) < std::get<partA ? 1 : 3>(y); });
            return std::get<0>(*max_it) * std::get<2>(*max_it);
        }

    }

    template<>
    void solver<AOC_YEAR, AOC_DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << result(in, std::true_type{});
    }

    template<>
    void solver<AOC_YEAR, AOC_DAY>::solveB(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << result(in, std::false_type{});
    }

    template<>
    void* solver<AOC_YEAR, AOC_DAY>::parse(std::istream& is) const
    {
        constexpr std::size_t NO_ID = std::numeric_limits<std::size_t>::max();

        std::istream::sentry s(is);
        if (!s) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };
        input_t& vec = *ptr;

        auto expect = [](std::istream& is, const auto& e)
        {
            if constexpr (std::is_same_v<stdx::remove_cvref_t<decltype(e)>, char>) {
                char x;
                if (!(is >> x)) throw parse_exception{};
                if (x != e)  throw parse_exception{};
            } else {
                char x;
                for (char a : e) {
                    if (!(is >> x)) throw parse_exception{};
                    if (x != a)  throw parse_exception{};
                }
            }
        };

        auto parse = [](std::istream& is, auto& x)
        {
            if ((is >> x).fail()) throw parse_exception{};
        };

        std::string line;
        while (std::getline(is, line))
        {
            if (line.empty()) continue;

            std::stringstream line_ss{ std::move(line) };
            auto& [t, id, action] = vec.emplace_back();
            id = NO_ID;

            std::intmax_t year, month, day, hour, min;

            expect(line_ss, '[');
            parse(line_ss, year);
            expect(line_ss, '-');
            parse(line_ss, month);
            expect(line_ss, '-');
            parse(line_ss, day);
            parse(line_ss, hour);
            expect(line_ss, ':');
            parse(line_ss, min);
            expect(line_ss, ']');
            t = {year, month, day, hour, min};

            std::string word;
            parse(line_ss, word);

            if (word[0] == 'G') { // begin shift
                expect(line_ss, '#');
                parse(line_ss, id);
                action = event_t::begin;
            } else if (word[0] == 'f') {
                action = event_t::sleep;
            } else {
                action = event_t::awake;
            }
        }

        if (vec.empty()) throw parse_exception{};

        std::sort(vec.begin(), vec.end(), [](const record_t& lhs, const record_t& rhs) { return std::get<date_t>(lhs) < std::get<date_t>(rhs); });

        std::size_t last_id = NO_ID;
        for (auto&[t, id, e] : vec) {
            if (e == event_t::begin) last_id = id;
            else id = last_id;
        }

        return ptr.release();
    }

    template<>
    void solver<AOC_YEAR, AOC_DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}