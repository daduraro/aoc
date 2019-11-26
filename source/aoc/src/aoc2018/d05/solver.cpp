#include "aoc/solver.h"
#include "aoc/error.h"
#include "version.h"

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <limits>
#include <cassert>

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = AOC_YEAR;
        constexpr std::size_t DAY = AOC_DAY;
        using input_t = std::string;

        // assume ascii, otherwise it should use locale
        char to_lower(char c) {
            assert((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
            return (c < 'a') ? c + 'a' - 'A' : c;
        }
        bool opposite(char a, char b) {
            return std::abs(a-b) == ('a' - 'A');
        }

        std::string reaction(std::string_view str) {
            std::string stack;
            for (char c : str) {
                if (stack.empty() || !opposite(stack.back(), c)) {
                    stack.push_back(c);
                } else { // reaction happened
                    stack.pop_back();
                }
            }

            return stack;
        }
    }

    template<>
    void solver<YEAR, DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        os << reaction(in).size();
    }

    template<>
    void solver<YEAR, DAY>::solveB(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
        const auto reacted_in = reaction(in);
        std::array<bool, 'z'-'a' + 1> has_unit{};
        for (auto c : reacted_in) has_unit[to_lower(c) - 'a'] = true;


        std::size_t min_size = std::numeric_limits<std::size_t>::max();
        for (std::size_t i = 0; i < has_unit.size(); ++i) {
            if (has_unit[i]) {
                std::string filtered;
                filtered.reserve(reacted_in.size());
                std::copy_if(reacted_in.begin(), reacted_in.end(), std::back_inserter(filtered), [e = i+'a'](char c) { return to_lower(c) != e; });
                min_size = std::min(min_size, reaction(filtered).size());
            }
        }

        os << min_size;
    }

    template<>
    void* solver<YEAR, DAY>::parse(std::istream& is) const
    {
        std::istream::sentry s(is);
        if (!s) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };
        input_t& str = *ptr;

        is >> str;
        if (is.fail()) throw parse_exception{};

        // ensure input belongs to a-zA-Z.
        if (!std::all_of(str.begin(), str.end(), [](char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); })) {
            throw parse_exception{};
        }

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}