#include "aoc/solver.h"

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
#include <bitset>
#include <regex>

namespace aoc {
namespace {
    constexpr std::size_t YEAR = 2018;
    constexpr std::size_t DAY  = 7;

    static constexpr std::size_t N = 'Z' - 'A' + 1;
    struct graph_t {
        std::array<std::bitset<N>, N> edges;
        std::bitset<N> vertices;
    };

    class tasks_t {
    public:
        tasks_t(std::bitset<N> tasks) noexcept : available_(), taken_(), done_(~tasks) {}
        explicit operator bool() const noexcept { return !done_.all(); }

        static constexpr std::size_t task_time(std::size_t t) { return t + 61; }

        [[nodiscard]] bool are_available() const noexcept {
            return available_.any();
        }

        [[nodiscard]] std::optional<std::size_t> next_available() const noexcept {
            for (std::size_t i = 0; i < N; ++i) if (available_[i]) return i;
            return std::nullopt;
        }

        void take(std::size_t t) noexcept {
            assert(available_[t]);
            available_.reset(t);
            taken_.set(t);
        }

        void mark_as_done(std::size_t t) noexcept {
            available_.reset(t);
            taken_.reset(t);
            done_.set(t);
        }

        void update(const graph_t& g) noexcept {
            auto to_do = ~done_;
            for (std::size_t t = 0; t < N; ++t) {
                auto impeding_tasks = g.edges[t];
                if ((impeding_tasks & to_do).none()) available_.set(t);
            }
            available_ &= ~taken_;
            available_ &= to_do;
        }

    private:
        std::bitset<N> available_;
        std::bitset<N> taken_;
        std::bitset<N> done_;
    };

    class worker_pool_t {
        static constexpr std::size_t INF_TIME = std::numeric_limits<std::size_t>::max();

    public:
        worker_pool_t(std::size_t workers) noexcept {
            assigned_task_.resize(workers);
            remaining_time_.resize(workers, INF_TIME);
        }

        [[nodiscard]] bool available(std::size_t worker) const noexcept {
            return !assigned_task_[worker].has_value();
        }

        [[nodiscard]] std::size_t num_workers() const noexcept {
            return assigned_task_.size();
        }

        std::size_t advance_time(tasks_t& tasks) noexcept
        {
            auto t = *std::min_element(remaining_time_.begin(), remaining_time_.end());
            for (auto& rt : remaining_time_) rt -= t;
            for (std::size_t w = 0; w < num_workers(); ++w) {
                if (remaining_time_[w] == 0) {
                    tasks.mark_as_done(*assigned_task_[w]);
                    assigned_task_[w] = std::nullopt;
                    remaining_time_[w] = INF_TIME;
                }
            }
            return t;
        }

        void assign_tasks(tasks_t& tasks) noexcept {
            for (std::size_t w = 0; w < num_workers(); ++w) {
                if (!available(w)) continue;
                if (auto t = tasks.next_available(); t) {
                    tasks.take(*t);
                    assigned_task_[w] = *t;
                    remaining_time_[w] = tasks.task_time(*t);
                }
            }
        }


    private:
        std::vector<std::optional<std::size_t>> assigned_task_;
        std::vector<std::size_t> remaining_time_;
    };

    using input_t = graph_t;

    input_t parse_input(std::istream& is) {
        input_t in;

        std::regex re { "Step ([A-Z]) must be finished before step ([A-Z]) can begin.", std::regex::ECMAScript };
        std::string line;
        while (std::getline(is, line)) {
            if (line.empty()) continue;

            if (std::smatch m ; std::regex_match(line, m, re)) {
                std::size_t a = m.str(1)[0] - 'A';
                std::size_t b = m.str(2)[0] - 'A';

                // relationship is a -> b
                in.vertices.set(a); in.vertices.set(b);
                in.edges[b].set(a);
            } else {
                throw parse_exception{};
            }
        }

        // check deadlocks
        std::bitset<N> visited;
        std::vector<std::size_t> stack;
        for (std::size_t i = 0; i < N; ++i) {
            if (visited[i]) continue;

            std::bitset<N> path;
            stack.push_back(i); // next node to visit

            while (!stack.empty()) {
                auto node = stack.back();
                if (visited[node]) {
                    // we have already visited the node, remove it from current path
                    // (if applicable) and do not explore it again
                    path.reset(node);
                    stack.pop_back();
                    continue;
                }

                visited.set(node);
                path.set(node);

                auto edges = in.edges[node];
                for (std::size_t next_node = 0; next_node < N; ++next_node) {
                    if (edges[next_node]) {
                        if (path[next_node]) throw parse_exception{ "graph contains cycles" };
                        stack.push_back(next_node);
                    }
                }
            }
        }
    }

    std::string resultA(const input_t& in) noexcept {
        std::string str;
        str.reserve(N);

        tasks_t tasks{ in.vertices };
        tasks.update(in);

        while (tasks.are_available()) { // there is still work to be done...
            auto next = *tasks.next_available(); // it will exists, for sure, as there is some available tasks
            str += char(next + 'A');
            tasks.mark_as_done(next);
            tasks.update(in);
        }

        return str;
    }

    std::size_t resultB(const input_t& in, std::size_t nworkers = 5) noexcept {
        std::size_t elapsed_time = 0;
        tasks_t tasks{ in.vertices };
        worker_pool_t workers{ nworkers };

        tasks.update(in);
        while (tasks) { // this loop will end as there are no deadlocks
            workers.assign_tasks(tasks);
            elapsed_time += workers.advance_time(tasks);
            tasks.update(in);
        }

        return elapsed_time;
    }
}

    template<>
    auto create_solver<YEAR, DAY>() noexcept -> std::unique_ptr<solver_interface> {
        return create_solver<YEAR, DAY>(parse_input, resultA, resultB);
    }
}