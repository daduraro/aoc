#ifndef INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6
#define INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6

#include <cstddef>
#include <iosfwd>

#include <aoc/serialize.h>
#include <aoc/error.h>

namespace aoc
{
    class solver_interface {
    public:
        virtual ~solver_interface() noexcept = default;
        virtual std::size_t year() const noexcept = 0;
        virtual std::size_t day() const noexcept = 0;
        virtual void* parse(std::istream& is) const = 0;
        virtual void solveA(std::ostream& os, const void* in) const noexcept = 0;
        virtual void solveB(std::ostream& os, const void* in) const noexcept = 0;
        virtual void cleanup(void*) const noexcept = 0;
    };

    int solve(int argc, char** argv, const solver_interface& solver) noexcept;

    template<std::size_t YEAR, std::size_t DAY>
    auto create_solver() noexcept -> std::unique_ptr<solver_interface>;

    // solver implementation based on three callbacks (parse, resultA, resultB)
    template<std::size_t YEAR, std::size_t DAY, typename I, typename A, typename B>
    class solver final : public solver_interface {
    public:
        using input_t = std::invoke_result_t<I, std::istream&>;
        template<typename FI, typename FA, typename FB>
        solver(FI&& i, FA&& a, FB&& b) noexcept
            : parse_input(std::forward<FI>(i)), resultA(std::forward<FA>(a)), resultB(std::forward<FB>(b)) {}
        ~solver() noexcept = default;
        virtual std::size_t year() const noexcept override { return YEAR; }
        virtual std::size_t day() const noexcept override { return DAY; }
        virtual void* parse(std::istream& is) const override
        {
            std::istream::sentry s(is);
            if (!s) throw parse_exception{ "invalid input stream" };
            return new input_t{ parse_input(is) };
        }
        virtual void solveA(std::ostream& os, const void* type_erased_in) const noexcept override
        {
            const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
            os << resultA(in);
        }
        virtual void solveB(std::ostream& os, const void* type_erased_in) const noexcept override
        {
            const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);
            os << resultB(in);
        }
        virtual void cleanup(void* ptr) const noexcept override
        {
            if constexpr (std::is_array_v<input_t>) delete[] reinterpret_cast<input_t*>(ptr);
            else delete reinterpret_cast<input_t*>(ptr);
        }

    private:
        // callbacks
        I parse_input;
        A resultA;
        B resultB;
    };

    // helper to create solver, could use deduction guide rules instead...
    template<std::size_t YEAR, std::size_t DAY, typename I, typename A, typename B>
    auto create_solver(I&& i, A&& a, B&& b) noexcept -> std::unique_ptr<solver_interface>
    {
        return std::unique_ptr<solver_interface>(
            new solver<YEAR, DAY, I, A, B>{
                std::forward<I>(i), std::forward<A>(a), std::forward<B>(b)
            });
    }
}

#endif // INCLUDE_GUARD__AOC_SOLVER_H__GUID_c89d6673_7083_4586_bb9f_528b6bb7d8b6