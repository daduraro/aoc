#ifndef INCLUDE_GUARD__DDR_MATH_VECTOR_H__GUID_a1e0be5d_78f5_42f7_8528_93ac0731a5fc
#define INCLUDE_GUARD__DDR_MATH_VECTOR_H__GUID_a1e0be5d_78f5_42f7_8528_93ac0731a5fc

#include <type_traits>
#include <array>
#include <algorithm>

namespace ddr::math {

    template<typename T>
    struct vec2 {
        static_assert(std::is_trivial_v<T>, "vec2 can hold only trivial types");
        using scalar_type = T;

        vec2() noexcept : x(), y() {};
        constexpr vec2(scalar_type x, scalar_type y) noexcept : x(x), y(y) {};
        vec2(const vec2& other) noexcept { std::memcpy(this, &other, sizeof(vec2)); }
        vec2& operator=(const vec2& other) noexcept { std::memcpy(this, &other, sizeof(vec2)); return *this; }

        friend constexpr bool operator==(const vec2& lhs, const vec2& rhs) noexcept { return (lhs.x == rhs.x) && (lhs.y == rhs.y); /*return std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), rhs.data.end());*/ }
        friend constexpr bool operator!=(const vec2& lhs, const vec2& rhs) noexcept { return !(lhs == rhs); }
        friend constexpr vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
        friend constexpr vec2 operator-(const vec2& lhs, const vec2& rhs) noexcept { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
        constexpr vec2& operator+=(const vec2& other) { x += other.x; y += other.y; return *this; }
        constexpr vec2& operator-=(const vec2& other) { x -= other.x; y -= other.y; return *this; }

        constexpr auto begin() noexcept { return data.begin(); }
        constexpr auto end() noexcept { return data.end(); }
        constexpr auto rbegin() noexcept { return data.rbegin(); }
        constexpr auto rend() noexcept { return data.rend(); }

        constexpr auto begin() const noexcept { return data.begin(); }
        constexpr auto end() const noexcept { return data.end(); }
        constexpr auto rbegin() const noexcept { return data.rbegin(); }
        constexpr auto rend() const noexcept { return data.rend(); }

        friend constexpr auto min_element(const vec2& x) { return std::min_element(x.data.begin(), x.data.end()); }
        friend constexpr auto max_element(const vec2& x) { return std::max_element(x.data.begin(), x.data.end()); }
        friend constexpr auto min_element(vec2& x) { return std::min_element(x.data.begin(), x.data.end()); }
        friend constexpr auto max_element(vec2& x) { return std::max_element(x.data.begin(), x.data.end()); }

        friend constexpr vec2 min(const vec2& lhs, const vec2& rhs) noexcept { return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y) }; }
        friend constexpr vec2 max(const vec2& lhs, const vec2& rhs) noexcept { return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y) }; }
        friend constexpr vec2 abs(const vec2& x) noexcept { return { std::abs(x.x), std::abs(x.y) }; }
        friend constexpr scalar_type manh(const vec2& x, const vec2& y) noexcept { auto v = abs(x-y); return v.x + v.y; }

        constexpr scalar_type& operator[](std::size_t i) { return data[i]; }
        constexpr const scalar_type& operator[](std::size_t i) const { return data[i]; }

        // do some nasty stuff... which obviously are undefined behaviour
        union {
            struct { scalar_type x; scalar_type y; };
            std::array<scalar_type, 2> data;
        };
    };

    template<std::size_t I, typename T> constexpr T& get(vec2<T>& v) { return std::get<I>(v.data); }
    template<std::size_t I, typename T> constexpr const T& get(const vec2<T>& v) { return std::get<I>(v.data); }
    template<std::size_t I, typename T> constexpr T&& get(vec2<T>&& v) { return std::get<I>(std::move(v.data)); }
    template<std::size_t I, typename T> constexpr const T&& get(const vec2<T>&& v) { return std::get<I>(std::move(v.data)); }

    using ivec2 = vec2<std::intmax_t>;
    using dvec2 = vec2<double>;
}

// specializations
namespace std {
    template<typename T> struct tuple_size<ddr::math::vec2<T>> : std::integral_constant<std::size_t, 2> {};
    template<std::size_t I, typename T> struct tuple_element<I, ddr::math::vec2<T>> { using type = T; };
}

#endif // INCLUDE_GUARD__DDR_MATH_VECTOR_H__GUUID_a1e0be5d_78f5_42f7_8528_93ac0731a5fc