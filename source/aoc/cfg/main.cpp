#include "aoc/solver.h"
#include "version.h"

int main(int argc, char** argv)
{
    using namespace aoc;
    return solve(argc, argv, solver<AOC_YEAR, AOC_DAY>{});
}