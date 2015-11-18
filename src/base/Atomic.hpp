#ifndef _BASE_ATOMIC_HPP
#define _BASE_ATOMIC_HPP

#include <cstdint>

namespace base {

    // Increase a int32 value by 1.
    inline void AtomicIncrease(volatile int32_t* ptr);

    // Decrease a int32 value by 1, and return whether the result is non-zero.
    inline bool AtomicDecrease(volatile int32_t* ptr);

}

#if defined(__GNUC__) || defined(__clang__)
    #include "atomic_gcc.hpp"
#elif defined(_WIN32) || defined(_MSC_VER)
    #include "Atomic_Win32.hpp"
#else
    #error "Unsupported compiler"
#endif

#endif // _BASE_ATOMIC_HPP
