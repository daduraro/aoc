#include "aoc/solver.h"
#include "aoc/error.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace aoc {

    namespace {

        struct config {
            std::unique_ptr<std::istream> input;
        };

        void print_exception(std::ostream& os, aoc_exception& e) {
            os << "an exception has occurred" << std::endl;
        }

        void print_exception(config& cfg, aoc_exception& e) {
            print_exception(std::cerr, e);
        }

        config parse_args(int argc, char** argv)
        {
            namespace fs = std::filesystem;

            if (argc != 2) {
                throw parse_exception{};
            }

            std::filesystem::path fpath = fs::u8path(argv[1]);
            if (!fs::exists(fpath)) throw parse_exception();

            return { std::make_unique<std::ifstream>(fpath, std::ios::in) };
        }

        int solve(config cfg, const solver_interface& solver) noexcept
        {
            try {
                solver(*cfg.input, std::cout, std::cout);
            }
            catch (aoc_exception& e) {
                print_exception(cfg, e);
                return 255;
            }
            return 0;
        }
    }

    int solve(int argc, char** argv, const solver_interface& solver) noexcept
    {
        try {
            return solve(parse_args(argc, argv), solver);
        }
        catch (aoc_exception& e) {
            print_exception(std::cerr, e);
            return 255;
        }
    }
}