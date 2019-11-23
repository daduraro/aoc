#ifndef INCLUDE_GUARD__AOC_ERROR_H__GUID_6f5c7275_79e7_4581_a3bd_11d138263d53
#define INCLUDE_GUARD__AOC_ERROR_H__GUID_6f5c7275_79e7_4581_a3bd_11d138263d53

#include <exception>

namespace aoc
{
    class aoc_exception : std::exception { using std::exception::exception; };

    class parse_exception : public aoc_exception { using aoc_exception::aoc_exception; };

}

#endif // INCLUDE_GUARD__AOC_ERROR_H__GUID_6f5c7275_79e7_4581_a3bd_11d138263d53