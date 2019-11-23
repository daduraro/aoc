#ifndef INCLUDE_GUARD__DDR_UTILS_META_H__GUID_f37768a5_279e_4411_b7d4_635f9d37c599
#define INCLUDE_GUARD__DDR_UTILS_META_H__GUID_f37768a5_279e_4411_b7d4_635f9d37c599

#include <type_traits>

namespace ddr::utils
{
    template<auto> constexpr bool always_false() { return false; }
    template<typename> constexpr bool always_false() { return false; }
}

#endif // INCLUDE_GUARD__DDR_UTILS_META_H__GUID_f37768a5_279e_4411_b7d4_635f9d37c599