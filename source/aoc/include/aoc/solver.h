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
        virtual std::size_t year() const noexcept = 0;
        virtual std::size_t day() const noexcept = 0;
        virtual void* parse(std::istream& is) const = 0;
        virtual void solveA(std::ostream& os, const void* in) const = 0;
        virtual void solveB(std::ostream& os, const void* in) const = 0;
        virtual void cleanup(void*) const noexcept = 0;
    };

    template<std::size_t YEAR, std::size_t DAY>
    class solver final : public solver_interface {
    public:
        ~solver() noexcept = default;
        virtual std::size_t year() const noexcept override { return YEAR; }
        virtual std::size_t day() const noexcept override { return DAY; }
        virtual void* parse(std::istream& is) const override;
        virtual void solveA(std::ostream& os, const void* in) const override;
        virtual void solveB(std::ostream& os, const void* in) const override;
        virtual void cleanup(void*) const noexcept;
    };

    int solve(int argc, char** argv, const solver_interface& solver) noexcept;
}

#endif // INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6