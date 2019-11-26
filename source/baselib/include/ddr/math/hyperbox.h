#ifndef INCLUDE_GUARD__DDR_MATH_HYPERBOX_H__GUID__6dfd14c0_2c9c_4305_89f9_cadd57e2e0c6
#define INCLUDE_GUARD__DDR_MATH_HYPERBOX_H__GUID__6dfd14c0_2c9c_4305_89f9_cadd57e2e0c6

#include <ddr/math/vector.h>

#include <functional>
#include <iterator>

namespace ddr::math {
    struct rect {
        ivec2 start;
        ivec2 size;

        struct iterator {
            using difference_type = std::ptrdiff_t;
            using value_type = ivec2;
            using pointer = const value_type*;
            using reference = const value_type&;
            using iterator_category = std::forward_iterator_tag;

            std::reference_wrapper<const rect> rect_;
            ivec2 idx_;

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
            return { *this, { start.x, start.y + size.y } };
        }
    };
}

#endif // INCLUDE_GUARD__DDR_MATH_HYPERBOX_H__GUID__6dfd14c0_2c9c_4305_89f9_cadd57e2e0c6