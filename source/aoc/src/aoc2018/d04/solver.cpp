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

#include <ddr/porting.h>

namespace {
    using namespace aoc;
    constexpr std::size_t YEAR = 2018;
    constexpr std::size_t  DAY = 4;

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

    input_t parse_input(std::istream& is)
    {
        constexpr std::size_t NO_ID = std::numeric_limits<std::size_t>::max();
        input_t vec;

        std::regex date_re{ R"re(^\[(\d+)-(\d+)-(\d+) (\d+):(\d+)\] )re" };
        std::regex fallsleep_re{ R"re(falls asleep)re" };
        std::regex wakeup_re{ R"re(wakes up)re" };
        std::regex shift_re{ R"re(Guard #(\d+) begins shift)re" };

        std::string line;
        while (std::getline(is, line))
        {
            if (line.empty()) continue;

            std::smatch date_match;
            if (!std::regex_search(line, date_match, date_re)) throw parse_exception{ "unexpected line format" };

            auto& [t, id, action] = vec.emplace_back();
            id = NO_ID;

            int year  = std::stoi(date_match[1].str());
            int month = std::stoi(date_match[2].str());
            int day   = std::stoi(date_match[3].str());
            int hour  = std::stoi(date_match[4].str());
            int min   = std::stoi(date_match[5].str());
            t = {year, month, day, hour, min};

            if (std::smatch action_match; std::regex_match( date_match[0].second, line.cend(), action_match, fallsleep_re)) action = event_t::sleep;
            else if (std::smatch action_match; std::regex_match( date_match[0].second, line.cend(), action_match, wakeup_re)) action = event_t::awake;
            else if (std::smatch action_match; std::regex_match( date_match[0].second, line.cend(), action_match, shift_re))
            {
                id = std::stoi(action_match[1].str());
                action = event_t::begin;
            }
        }

        if (vec.empty()) throw parse_exception{};

        std::sort(vec.begin(), vec.end(), [](const record_t& lhs, const record_t& rhs) { return std::get<date_t>(lhs) < std::get<date_t>(rhs); });
        if (std::get<event_t>(vec[0]) !=  event_t::begin) throw parse_exception{ "first event is not a guard beginning their shift" };

        std::size_t last_id = NO_ID;
        for (auto&[t, id, e] : vec) {
            if (e == event_t::begin) last_id = id;
            else id = last_id;
        }

        return vec;
    }

    template<bool partA>
    std::size_t result(input_t in) noexcept
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

namespace aoc {
    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, result<true>, result<false>);
    }
}