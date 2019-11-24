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

namespace aoc
{
    namespace {
        constexpr std::size_t YEAR = 2018;
        constexpr std::size_t DAY = 02;
        using input_t = std::vector<std::string>;

        struct trie_t {
            struct node_t {
                std::array<std::unique_ptr<node_t>, 26> children;
            };

            static constexpr char idx(char ch) noexcept
            {
                ch -= 'a';
                assert(ch >= 0 && ch < 26);
                return ch;
            }

            void insert(std::string_view str) noexcept {
                auto* node = &root;
                for (std::size_t i = 0; i < str.size(); ++i) {
                    char ch = idx(str[i]);
                    assert(ch >= 0 && ch < 26);
                    if (node->children[ch] != nullptr) node = node->children[ch].get();
                    else {
                        // insert remaining nodes
                        do {
                            ch = idx(str[i]);
                            node->children[ch] = std::make_unique<node_t>();
                            node = node->children[ch].get();
                        } while (++i < str.size());
                    }
                }
            }

            friend std::optional<std::string> find_similar(const node_t* node, std::string_view str, std::size_t dissimilarity) noexcept {
                std::string path;
                path.reserve(str.size());

                for (std::size_t i = 0; i < str.size(); ++i) {
                    char ch = str[i];
                    auto* next = node->children[idx(ch)].get();
                    if (next != nullptr) {
                        path += ch;
                        node = next;
                        continue;
                    }
                    if (dissimilarity == 0) return std::nullopt;

                    for (const auto& child : node->children) {
                        if (child == nullptr) continue;
                        auto result = find_similar(child.get(), str.substr(i+1), dissimilarity-1);
                        if (result) return path + result.value();
                    }
                    break;
                }

                if (dissimilarity == 0) return path;

                return std::nullopt;
            }

            std::optional<std::string> find(std::string_view str) const noexcept
            {
                return find_similar(&root, str, 1);
            }

            node_t root;
        };

        std::optional<std::string> resultB(const input_t& in) noexcept
        {
            trie_t trie;
            assert(!in.empty());
            trie.insert(in[0]);
            for (std::size_t i = 1; i < in.size(); ++i) {
                const auto& str = in[i];
                auto result = trie.find(str);
                if (result) return result;
                trie.insert(str);
            }
            return std::nullopt;
        }
    }

    template<>
    void solver<YEAR, DAY>::solveA(std::ostream& os, const void* type_erased_in) const
    {
        const input_t& in = *reinterpret_cast<const input_t*>(type_erased_in);

        std::size_t exactly_2 = 0;
        std::size_t exactly_3 = 0;
        for (std::string s : in) {
            std::sort(s.begin(), s.end());

            bool has_two = false;
            bool has_three = false;
            auto i = s.begin();
            decltype(i) j;
            while (i != s.end()) {
                auto j = std::adjacent_find(i, s.end(), [](const auto& lhs, const auto& rhs) { return lhs != rhs; });
                std::advance(j, j != s.end());
                auto ncount = std::distance(i, j);
                if (ncount == 2) has_two = true;
                else if (ncount == 3) has_three = true;
                i = j;
            }
            exactly_2 += std::size_t(has_two);
            exactly_3 += std::size_t(has_three);
        }

        os << exactly_2 * exactly_3;
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
        std::unique_ptr<input_t> ptr{ new input_t{} };

        input_t& vec = *ptr;

        using stream_iter = std::istream_iterator<std::string>;
        std::copy(stream_iter(is), stream_iter(), std::back_inserter(vec));
        if (vec.empty() || !is.eof()) throw parse_exception{};

        // guarantee all boxes ID are of same size and with characters in 'a'-'z' range (lowercase)
        std::size_t size_id = vec[0].size();
        for (const auto& s : vec) {
            if (s.size() != size_id || s.end() != std::find_if(s.begin(), s.end(), [](char ch) { return ch < 'a' || ch > 'z'; }))
                throw parse_exception{};
        }

        return ptr.release();
    }

    template<>
    void solver<YEAR, DAY>::cleanup(void* ptr) const noexcept
    {
        delete reinterpret_cast<input_t*>(ptr);
    }

}