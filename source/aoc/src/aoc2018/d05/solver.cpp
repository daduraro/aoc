#include "aoc/solver.h"

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
        constexpr std::size_t YEAR = 2018;
        constexpr std::size_t  DAY = 5;
        using input_t = std::string;

        input_t parse_input(std::istream& is)
        {
            input_t str;

            is >> str;
            if (is.fail()) throw parse_exception{};

            // ensure input belongs to a-zA-Z.
            if (!std::all_of(str.begin(), str.end(), [](char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); })) {
                throw parse_exception{};
            }

            return str;
        }

        // assume ascii, otherwise it should use locale
        char to_lower(char c) noexcept {
            assert((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
            return (c < 'a') ? c + 'a' - 'A' : c;
        }
        bool opposite(char a, char b) noexcept {
            return std::abs(a-b) == ('a' - 'A');
        }

        std::string reaction(std::string_view str) noexcept {
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

        std::size_t resultA(const input_t& in) noexcept {
            return reaction(in).size();
        }

        std::size_t resultB(const input_t& in) noexcept {
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

            return min_size;
        }
    }

    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}