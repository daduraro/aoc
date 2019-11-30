#include "aoc/solver.h"
#include "aoc/error.h"
#include "version.h"

#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <limits>
#include <type_traits>
#include <cassert>

#include <ddr/data/tree.h>

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = AOC_YEAR;
        constexpr std::size_t DAY  = AOC_DAY;

        using node_t = ddr::data::node_t<std::vector<std::intmax_t>>;
        using input_t = node_t;

        template<typename T>
        std::ostream& operator<<(std::ostream& os, const std::optional<T>& r) noexcept {
            if (std::ostream::sentry s(os); s) {
                if (r) os << r.value();
                else   os << "N/A";
            }
            return os;
        }

        std::size_t resultA(const input_t& in) {
            std::vector<const node_t*> nodes;
            nodes.push_back(&in);
            std::intmax_t accum = 0;
            while (!nodes.empty()) {
                const node_t& node = *nodes.back();
                nodes.pop_back();
                accum += std::reduce(node->begin(), node->end());
                for (const node_t& child : node.children) nodes.push_back(&child);
            }
            return std::size_t(accum);
        }

        std::size_t resultB(const node_t& in)
        {
            if (in.children.empty()) {
                auto sum = std::reduce(in->begin(), in->end());
                return std::size_t(sum);
            } else {
                std::vector<std::optional<std::size_t>> cached_values;
                cached_values.resize(in.children.size());
                std::size_t accum = 0;
                for (auto child : *in)
                {
                    if (child == 0 || std::size_t(child) > in.children.size()) continue; // invalid child
                    auto& value = cached_values[child-1];
                    if (!value) value = resultB(in.children[child - 1]);
                    accum += *value;
                }
                return accum;
            }
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
        os << resultB(in);
    }

    template<>
    void* solver<YEAR, DAY>::parse(std::istream& is) const
    {
        std::istream::sentry s(is);
        if (!s) throw parse_exception{};

        std::unique_ptr<input_t> ptr{ new input_t{} };
        input_t& root = *ptr;

        auto read_tree = [&is](node_t& node) -> void {
            auto impl = [&is](const auto& self, node_t& node) -> void {
                std::size_t num_children, num_data;
                if (!(is >> num_children >> num_data)) throw parse_exception{};
                node.children.resize(num_children);
                for (node_t& child : node.children) self(self, child);
                std::copy_n(std::istream_iterator<std::intmax_t>(is), num_data, std::back_inserter(*node));
                if (is.fail()) throw parse_exception{};
            };
            impl(impl, node);
        };
        read_tree(root);

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        if constexpr (std::is_array_v<input_t>) delete[] reinterpret_cast<input_t*>(ptr);
        else delete reinterpret_cast<input_t*>(ptr);
    }
}