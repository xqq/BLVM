#ifndef _BASE_ATOMIC_WIN32_HPP
#define _BASE_ATOMIC_WIN32_HPP

#include <cstdint>
#include <intrin.h>

namespace base {

    inline void AtomicIncrease(volatile int32_t* ptr) {
        _InterlockedIncrement(reinterpret_cast<volatile long*>(ptr));
    }

    inline bool AtomicDecrease(volatile int32_t* ptr) {
        return 0 != _InterlockedDecrement(reinterpret_cast<volatile long*>(ptr));
    }

}

#endif // _BASE_ATOMIC_WIN32_HPP
