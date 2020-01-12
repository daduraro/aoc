#include "aoc/solver.h"

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

namespace {
    using namespace aoc;
    constexpr std::size_t YEAR = 2018;
    constexpr std::size_t DAY  = 8;

    using node_t = ddr::data::node_t<std::vector<std::intmax_t>>;
    using input_t = node_t;

    input_t parse_input(std::istream& is)
    {
        input_t root;

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

        return root;
    }

    std::size_t resultA(const input_t& in) noexcept
    {
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

    std::size_t resultB(const node_t& in) noexcept
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

namespace aoc {
    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}