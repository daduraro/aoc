#include "aoc/solver.h"
#include "aoc/error.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <sstream>
#include <string>
#include <iomanip>
#include <map>
#include <functional>
#include <memory>

namespace aoc {

    namespace {
        struct annotated_fstream {
            std::filesystem::path fpath;
            std::ifstream stream;
        };
        struct problem {
            annotated_fstream input;
            std::optional<annotated_fstream> compareA;
            std::optional<annotated_fstream> compareB;
        };

        struct config {
            config(std::ostream& stream) noexcept : stream(stream) {};
            std::vector<problem> dataset;
            std::ostream& stream;
        };

        bool equal(std::istream& is, std::string_view str, std::ostream& os) noexcept
        {
            std::stringstream gt_ss;
            is.get(*gt_ss.rdbuf()); // read until \n
            auto groundtruth = std::move(gt_ss).str(); // sad copy
            str = str.substr(0, str.find('\n')); // read until \n

            if (groundtruth != str) {
                os << "Expected '" << groundtruth << "' but found '" << str << "'\n";
                return false;
            }
            return true;
        }

        void print_exception(std::ostream& os, aoc_exception& e) noexcept {
            os << "an exception has occurred" << std::endl;
        }

        void print_exception(config& cfg, aoc_exception& e) noexcept {
            print_exception(std::cerr, e);
        }

        config parse_args(int argc, char** argv, std::size_t year, std::size_t day)
        {
            namespace fs = std::filesystem;

            if (argc != 2) {
                throw parse_exception{};
            }

            std::filesystem::path fpath = fs::u8path(argv[1]);

            // load all *.in, *.outA, *.outB files inside, e.g., "aoc18/d01"
            fpath /= "aoc";
            fpath += std::to_string(year % 100);
            fpath /= "d";
            std::stringstream ss;
            ss << std::fixed << std::setfill('0') << std::setw(2) << std::right << day; // std::format...
            fpath += ss.str();

            if (!fs::exists(fpath)) throw parse_exception();

            // find all *.in files
            config cfg{ std::cout };
            std::map<std::string, problem> problems;
            for (const auto& s : fs::directory_iterator(fpath)) {
                auto p = s.path();
                auto ext = p.extension().u8string();
                if (s.is_regular_file() && ext == ".in") {
                    problem pr;
                    pr.input = { p, std::ifstream{ p } };
                    problems[p.stem().u8string()] = std::move(pr);
                }
            }

            for (const auto& s : fs::directory_iterator(fpath)) {
                auto p = s.path();
                if (s.is_regular_file()) {
                    std::string ext = p.extension().u8string();
                    if (ext != ".outA" && ext != ".outB") continue;
                    std::string stem = p.stem().u8string();

                    auto it = problems.find(stem);
                    if (it == problems.end()) continue;
                    if (ext == ".outA") it->second.compareA = { p, std::ifstream{p} };
                    else it->second.compareB = { p, std::ifstream{p} };
                }
            }

            if (problems.empty()) throw parse_exception();

            cfg.dataset.reserve(problems.size());
            for (auto&[_, pr] : problems) cfg.dataset.push_back(std::move(pr));

            return cfg;
        }

        int solve(config cfg, const solver_interface& solver) noexcept
        {
            for (auto& problem : cfg.dataset) {
                std::ostream& os = cfg.stream;
                os << std::setw(20) << std::left << problem.input.fpath.filename();
                std::unique_ptr<void, std::function<void(void*)>> input{ nullptr, [&solver](void* ptr){ solver.cleanup(ptr); }};
                try {
                    input.reset(solver.parse(problem.input.stream));
                }
                catch (aoc_exception&) {
                    os << "ERROR - could not parse input" << std::endl;
                    return 255;
                }

                std::stringstream errors;
                bool errors_happened = false;

                try {
                    os << "partA: ";
                    os << std::setw(10) << std::right;
                    if (problem.compareA) {
                        std::stringstream buffer;
                        solver.solveA(buffer, input.get());
                        if (equal(problem.compareA->stream, buffer.str(), errors)) {
                            os << "[PASSED]";
                        } else {
                            os << "[FAILED]";
                        }
                    } else {
                        solver.solveA(os, input.get());
                    }
                }
                catch (aoc_exception& e) {
                    os << "[ERROR]";
                    errors << "ERROR while solving partA:\n";
                    print_exception(errors, e);
                    errors_happened = true;
                }
                os << "    ";
                try {
                    os << "partB: ";
                    os << std::setw(10) << std::right;
                    if (problem.compareB) {
                        std::stringstream buffer;
                        solver.solveB(buffer, input.get());
                        if (equal(problem.compareB->stream, buffer.str(), errors)) {
                            os << "[PASSED]";
                        } else {
                            os << "[FAILED]";
                        }
                    } else {
                        solver.solveB(os, input.get());
                    }
                }
                catch (aoc_exception& e) {
                    os << "[ERROR]";
                    errors << "ERROR while solving partB:\n";
                    print_exception(errors, e);
                    errors_happened = true;
                }
                os << "\n";
                if (errors_happened) os << errors.rdbuf();
                os << std::flush;
            }
            return 0;
        }
    }

    int solve(int argc, char** argv, const solver_interface& solver) noexcept
    {
        try {
            return solve(parse_args(argc, argv, solver.year(), solver.day()), solver);
        }
        catch (aoc_exception& e) {
            print_exception(std::cerr, e);
            return 255;
        }
    }
}