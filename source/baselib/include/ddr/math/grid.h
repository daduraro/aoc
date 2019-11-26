#ifndef INCLUDE_GUARD__DDR_MATH_GRID_H__GUID_f45efb14_9d47_49c1_8b3b_652f34cda8dd
#define INCLUDE_GUARD__DDR_MATH_GRID_H__GUID_f45efb14_9d47_49c1_8b3b_652f34cda8dd

#include <ddr/math/vector.h>

#include <vector>
#include <cassert>

namespace ddr::math {

    template<typename Cell>
    class grid2 : private std::vector<Cell> {
        using base_type = std::vector<Cell>;
        using index_type = ivec2;
        using size_type = index_type;
    public:
        grid2() noexcept = default;
        grid2(index_type size) noexcept(noexcept(Cell{}))
        {
            base_type& vec = static_cast<base_type&>(*this);
            assert(size[0] > 0);
            assert(size[1] > 0);
            vec.resize(std::size_t(size[0] * size[1]));
            bounds_ = size;
        }

        void resize(const index_type& size) {
            base_type& vec = static_cast<base_type&>(*this);
            vec.clear();
            assert(size[0] > 0);
            assert(size[1] > 0);
            vec.resize(std::size_t(size[0] * size[1]));
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

}

#endif // INCLUDE_GUARD__DDR_MATH_GRID_H__GUID_f45efb14_9d47_49c1_8b3b_652f34cda8dd