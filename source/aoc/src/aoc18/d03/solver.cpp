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

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = 2018;
        constexpr std::size_t DAY = 03;

        template<typename T = std::intmax_t>
        struct vec2 {
            static_assert(std::is_trivial_v<T>, "vec2 can only hold trivial types");
            using scalar_type = T;
            vec2() noexcept : x(), y() {};
            vec2(scalar_type x, scalar_type y) noexcept : x(x), y(y) {};
            vec2(const vec2& other) noexcept { std::memcpy(this, &other, sizeof(vec2)); }
            vec2& operator=(const vec2& other) { std::memcpy(this, &other, sizeof(vec2)); return *this; }

            scalar_type& operator[](std::size_t i) { return data[i]; }
            const scalar_type& operator[](std::size_t i) const { return data[i]; }

            // do some nasty stuff... which obviously are undefined behaviour
            union {
                struct { scalar_type x; scalar_type y; };
                std::array<scalar_type, 2> data;
            };
        };

        struct rect_t {
            std::size_t id;
            vec2<std::intmax_t> start;
            vec2<std::intmax_t> size;
        };

        using input_t = std::vector<rect_t>;

        template<typename Cell>
        class board_t : private std::vector<Cell> {
            using base_type = std::vector<Cell>;
            using index_type = vec2<std::intmax_t>;
            using size_type = index_type;
        public:
            board_t() noexcept = default;
            board_t(index_type size) noexcept(noexcept(Cell{}))
            {
                base_type& vec = static_cast<base_type&>(*this);
                vec.resize(size[0] * size[1]);
                bounds_ = size;
            }

            void resize(const index_type& size) {
                base_type& vec = static_cast<base_type&>(*this);
                vec.clear();
                vec.resize(size[0] * size[1]);
                bounds_ = size;
            }

            Cell& operator[](const index_type& idx)
            {
                return static_cast<base_type&>(*this)[linear_index(idx)];
            }

            const Cell& operator[](const index_type& idx) const
            {
                return static_cast<const base_type&>(*this)[linear_index(idx)];
            }

            std::size_t linear_index(const index_type& idx) const noexcept {
                return std::size_t(idx.y * bounds_.x + idx.x);
            }

            using base_type::begin;
            using base_type::end;
            using base_type::rbegin;
            using base_type::rend;

        private:
            size_type bounds_;
        };

        std::size_t resultA(const input_t& in) noexcept
        {
            board_t<std::size_t> board;

            // resize board to hold all rects
            {
                vec2 max;
                for (const rect_t& r : in) {
                    max.x = std::max(max.x, r.start.x + r.size.x);
                    max.y = std::max(max.y, r.start.y + r.size.y);
                }
                board.resize(max);
            }

            for (const rect_t& r : in) {
                for (auto y = r.start.y; y < r.start.y + r.size.y; ++y)
                    for (auto x = r.start.x; x < r.start.x + r.size.x; ++x)
                        ++board[{x,y}];
            }

            return std::count_if(board.begin(), board.end(), [](auto n) { return n > 1; });
        }

        std::optional<std::size_t> resultB(const input_t& in) noexcept
        {
            board_t<std::size_t> board;

            // resize board to hold all rects
            {
                vec2 max;
                for (const rect_t& r : in) {
                    max.x = std::max(max.x, r.start.x + r.size.x);
                    max.y = std::max(max.y, r.start.y + r.size.y);
                }
                board.resize(max);
            }

            for (const rect_t& r : in) {
                for (auto y = r.start.y; y < r.start.y + r.size.y; ++y)
                    for (auto x = r.start.x; x < r.start.x + r.size.x; ++x)
                        ++board[{x,y}];
            }

            for (const rect_t& r : in) {
                for (auto y = r.start.y; y < r.start.y + r.size.y; ++y) {
                    for (auto x = r.start.x; x < r.start.x + r.size.x; ++x) {
                        if (board[{x,y}] != 1) goto next_iter;
                    }
                }
                return r.id;
                next_iter: continue;
            }
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
        if (!is) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };

        input_t& vec = *ptr;

        auto expect = [](std::istream& is, char c)
        {
            char e;
            if (!(is >> e)) throw parse_exception{};
            if (e != c)  throw parse_exception{};
        };

        std::string line;
        while (std::getline(is, line))
        {
            if (line.empty()) continue;

            std::stringstream line_ss{ std::move(line) };
            rect_t r;
            expect(line_ss, '#');
            if (!(line_ss >> r.id)) throw parse_exception{};
            expect(line_ss, '@');
            if (!(line_ss >> r.start.x)) throw parse_exception{};
            expect(line_ss, ',');
            if (!(line_ss >> r.start.y)) throw parse_exception{};
            expect(line_ss, ':');
            if (!(line_ss >> r.size.x)) throw parse_exception{};
            expect(line_ss, 'x');
            if (!(line_ss >> r.size.y).eof()) throw parse_exception{};

            if (r.start.x < 0 || r.start.y < 0 || r.size.x < 0 || r.size.y < 0)
                throw parse_exception{};

            vec.push_back(r);
        }

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}