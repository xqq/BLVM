#ifndef _BLVM_BASE_ATOMIC_GCC_HPP
#define _BLVM_BASE_ATOMIC_GCC_HPP

#include <cstdint>

namespace blvm {
namespace base {

    inline void AtomicIncrease(volatile int32_t* ptr) {
        __sync_fetch_and_add(ptr, 1);
    }

    inline bool AtomicDecrease(volatile int32_t* ptr) {
        return 0 != __sync_sub_and_fetch(ptr, 1);
    }

}
}

#endif // _BLVM_BASE_ATOMIC_GCC_HPP
