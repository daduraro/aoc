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
        constexpr std::size_t YEAR = 2018;
        constexpr std::size_t DAY = 04;

        class date_t {
        public:
            // regular fake dates, where months have 31 days, and a year always have 12 months
            constexpr date_t() noexcept = default;
            constexpr date_t(std::intmax_t mins_since_epoch) noexcept : mins_since_epoch(mins_since_epoch) {};
            constexpr date_t(std::intmax_t years, std::intmax_t months, std::intmax_t days, std::intmax_t hours, std::intmax_t minutes) noexcept
                : mins_since_epoch(minutes + 60 * (hours + 24 * (days + 31 * (months + 12 * years)))) {};
            constexpr unsigned min() const noexcept { return unsigned(((mins_since_epoch % 60) + 60) % 60); }

            friend constexpr bool operator==(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch == rhs.mins_since_epoch; }
            friend constexpr bool operator!=date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch != rhs.mins_since_epoch; }
            friend constexpr bool operator<(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch < rhs.mins_since_epoch; }
            friend constexpr bool operator<=(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch <= rhs.mins_since_epoch; }
            friend constexpr bool operator>(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch > rhs.mins_since_epoch; }
            friend constexpr bool operator>=(date_t lhs, date_t rhs) noexcept { return lhs.mins_since_epoch >= rhs.mins_since_epoch; }

        private:
            std::intmax_t mins_since_epoch;
        };

        enum class event_t {
            begin,
            sleep,
            awake
        };

        using record_t = std::tuple<date_t, std::size_t, event_t>;
        using input_t = std::vector<record_t>;

        std::size_t resultA(const input_t& in) noexcept
        {
        }

        std::optional<std::size_t> resultB(const input_t& in) noexcept
        {
            return std::nullopt;
        }
    }

    template<>
    void solver<YEAR, DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << resultA(in);
    }

    template<>
    void solver<YEAR, DAY>::solveB(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        auto result = resultB(in);
        if (result) os << *result;
        else os << "N/A";
    }

    template<>
    void* solver<YEAR, DAY>::parse(std::istream& is) const
    {
        constexpr std::size_t NO_ID = std::numeric_limits<std::size_t>::max();

        std::istream::sentry s(is);
        if (!s) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };
        input_t& vec = *ptr;

        auto expect = [](std::istream& is, const auto& c)
        {
            if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>, char>) {
                char e;
                if (!(is >> e)) throw parse_exception{};
                if (e != c)  throw parse_exception{};
            } else {
                char e;
                for (char x : c) {
                    if (!(is >> e)) throw parse_exception{};
                    if (e != x)  throw parse_exception{};
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
            record_t r;
            auto& [t, id, action] = r;
            id = NO_ID;

            std::intmax_t year, month, day, hour, min;

            expect(line_ss, '[');
            parse(line_ss, year);
            expect(line_ss, '-');
            parse(line_ss, month);
            expect(line_ss, '-');
            parse(line_ss, day);
            expect(line_ss, ' ');
            parse(line_ss, hour);
            expect(line_ss, ':');
            parse(line_ss, min);
            expect(line_ss, ']');

            std::string word;
            parse(line_ss, word);

            if (word[0] == 'G') { // begin shift
                expect(line_ss, " #");
                parse(line_ss, id);
                action = event_t::begin;
            } else if (word[0] == 'f') {
                action = event_t::sleep;
            } else {
                action = event_t::awake;
            }

            vec.push_back(r);
        }

        std::sort(vec.begin(), vec.end(), [](const record_t& lhs, const record_t& rhs) { return std::get<date_t>(lhs) < std::get<date_t>(rhs); });

        std::size_t last_id = NO_ID;
        for (auto&[t, id, e] : vec) {
            if (e == event_t::begin) last_id = id;
            else id = last_id;
        }

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}