#ifndef INCLUDE_GUARD__DDR_UTILS_OPTIONAL_REF_H__GUID_9d2aa945_ba99_4be5_a0fa_24879e8ebc18
#define INCLUDE_GUARD__DDR_UTILS_OPTIONAL_REF_H__GUID_9d2aa945_ba99_4be5_a0fa_24879e8ebc18

#include <optional>
#include <functional>

namespace ddr::utils
{
    template<typename T> using optional_ref = std::optional<std::reference_wrapper<T>>;
}

#endif // INCLUDE_GUARD__DDR_UTILS_OPTIONAL_REF_H__GUID_9d2aa945_ba99_4be5_a0fa_24879e8ebc18