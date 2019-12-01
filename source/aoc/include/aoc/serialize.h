#ifndef INCLUDE_GUARD__AOC_SERIALIZE_H__GUID_92f054b0_7716_44d5_8f4f_ec112e5e3d36
#define INCLUDE_GUARD__AOC_SERIALIZE_H__GUID_92f054b0_7716_44d5_8f4f_ec112e5e3d36

#include <optional>
#include <iostream>

namespace aoc {
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const std::optional<T>& r) noexcept {
        if (std::ostream::sentry s(os); s) {
            if (r) os << r.value();
            else   os << "N/A";
        }
        return os;
    }
}

#endif // INCLUDE_GUARD__AOC_SERIALIZE_H__GUID_92f054b0_7716_44d5_8f4f_ec112e5e3d36