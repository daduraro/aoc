#ifndef INCLUDE_GUARD__DDR_DATA_TREE_H__GUID_8bac1f36_92ed_4d75_8fba_cc0859a0a81a
#define INCLUDE_GUARD__DDR_DATA_TREE_H__GUID_8bac1f36_92ed_4d75_8fba_cc0859a0a81a

#include <vector>

namespace ddr::data {

    template<typename T>
    struct node_t {
        std::vector<node_t> children;
        T data;

        T& operator*() noexcept { return data; }
        const T& operator*() const noexcept { return data; }
        T* operator->() noexcept { return &data; }
        const T* operator->() const noexcept { return &data; }
    };

};

#endif // INCLUDE_GUARD__DDR_DATA_TREE_H__GUID_8bac1f36_92ed_4d75_8fba_cc0859a0a81a