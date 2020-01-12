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
#include <cctype>

namespace {
    using namespace aoc;
    constexpr std::size_t YEAR = 2019;
    constexpr std::size_t DAY  = 2;
    enum opcode_enum : std::intmax_t {
        OP_SUM = 1,
        OP_PRODUCT = 2,
        OP_HALT = 99,
        OP_ERROR = -1
    };
    using input_t = std::vector<std::intmax_t>;

    input_t parse_input(std::istream& is)
    {
        input_t in;
        std::string str;
        while (std::getline(is, str, ',')) {
            std::size_t pos;
            auto value = std::stoll(str, &pos);
            if (std::any_of(std::next(str.begin(), pos), str.end(), [](auto ch) { return !std::isspace(ch); }))
                throw parse_exception{};
            in.push_back(value);
        }
        if (!is.eof()) throw parse_exception{};
        if (in.empty()) throw parse_exception{};
        in.resize(((in.size() + 3)>>2)<<2, OP_ERROR);
        return in;
    }

    auto run(input_t in) noexcept -> std::optional<std::intmax_t>
    {
        for (std::size_t PC = 0; PC < in.size(); PC += 4)
        {
            switch (in[PC]) {
            case OP_SUM:
            case OP_PRODUCT:
            {
                // bound checking
                if (in[PC + 1] < 0 || std::size_t(in[PC + 1]) >= in.size()) return std::nullopt;
                if (in[PC + 2] < 0 || std::size_t(in[PC + 2]) >= in.size()) return std::nullopt;
                if (in[PC + 3] < 0 || std::size_t(in[PC + 3]) >= in.size()) return std::nullopt;

                auto x = in[std::size_t(in[PC + 1])];
                auto y = in[std::size_t(in[PC + 2])];
                auto& to = in[std::size_t(in[PC + 3])];
                to = in[PC] == OP_SUM ? x + y : x * y;
                break;
            }
            case OP_HALT: goto end_loop__;
            default: return std::nullopt;
            }
        }
        return std::nullopt;

        end_loop__:
        return in[0];
    }

    auto resultA(input_t in) noexcept -> std::optional<std::intmax_t>
    {
        // first instruction replacement
        in[1] = 12;
        in[2] = 2;
        return run(std::move(in));
    }

    auto resultB(input_t in) noexcept -> std::optional<std::size_t>
    {
        // could be done using symbolic computation,
        // but it should be done in care as opcode could be affected
        // by the input values...
        for (std::intmax_t noun = 0; noun < 100; ++noun)
            for (std::intmax_t verb = 0; verb < 100; ++verb) {
                in[1] = noun;
                in[2] = verb;
                auto result = run(in);
                if (result.value_or(0) == 19690720) {
                    return std::size_t(100 * noun + verb);
                }
            }
        return std::nullopt;
    }
}

namespace aoc {
    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}