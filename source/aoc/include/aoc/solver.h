#ifndef INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6
#define INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6

#include <cstddef>
#include <iosfwd>

#include <ddr/utils/optional_ref.h>

namespace aoc
{
    class solver_interface {
    public:
        virtual ~solver_interface() noexcept = default;
        virtual void operator()(std::istream& is, ddr::utils::optional_ref<std::ostream> partA, ddr::utils::optional_ref<std::ostream> partB) const = 0;
    };

    template<std::size_t year, std::size_t day>
    class solver final : public solver_interface {
    public:
        ~solver() noexcept = default;
        virtual void operator()(std::istream& is, ddr::utils::optional_ref<std::ostream> partA, ddr::utils::optional_ref<std::ostream> partB) const override;
    };

    int solve(int argc, char** argv, const solver_interface& solver) noexcept;
}

#endif // INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6