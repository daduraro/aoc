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
            vec2& operator=(const vec2& other) noexcept { std::memcpy(this, &other, sizeof(vec2)); return *this; }
            friend bool operator==(const vec2& lhs, const vec2& rhs) noexcept { return std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), rhs.data.end()); }
            friend bool operator!=(const vec2& lhs, const vec2& rhs) noexcept { return !(lhs == rhs); }
            friend vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept { return { lhs.x + rhs.x, lhs.y + rhs.y }; }

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

            struct iterator {
                using difference_type = std::ptrdiff_t;
                using value_type = vec2<std::intmax_t>;
                using pointer = const value_type*;
                using reference = const value_type&;
                using iterator_category = std::forward_iterator_tag;

                std::reference_wrapper<const rect_t> rect_;
                vec2<std::intmax_t> idx_;

                iterator& operator++() noexcept {
                    auto remquo = std::div(idx_.x + 1 - rect_.get().start.x, rect_.get().size.x);
                    idx_.x = remquo.rem + rect_.get().start.x;
                    idx_.y += remquo.quot;
                    return *this;
                }
                iterator operator++(int) noexcept { auto cpy = *this; ++(*this); return cpy; }
                friend bool operator==(const iterator& lhs, const iterator& rhs) noexcept { return lhs.idx_ == rhs.idx_; }
                friend bool operator!=(const iterator& lhs, const iterator& rhs) noexcept { return !(lhs == rhs); }
                reference operator*() const noexcept { return idx_; }
                pointer operator->() const noexcept { return &idx_; }
            };
            friend struct iterator;

            iterator begin() const noexcept { return { *this, start }; }
            iterator end() const noexcept {
                return { *this, {start.x, start.y + size.y} };
            }
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

            for (const rect_t& r : in)
                for (const auto& idx : r)
                    ++board[idx];

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

            for (const rect_t& r : in)
                for (const auto& idx : r)
                    ++board[idx];

            for (const rect_t& r : in) {
                bool found = true;
                for (const auto& idx : r) {
                    if (board[idx] != 1) {
                        found = false;
                        break;
                    }
                }
                if (found) return r.id;
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