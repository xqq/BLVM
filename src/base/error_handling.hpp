#ifndef _BLVM_BASE_ERROR_HANDLING_HPP
#define _BLVM_BASE_ERROR_HANDLING_HPP

#include <cassert>

#ifndef NDEBUG
    #define DCHECK(condition) assert(condition)
#else
    #define DCHECK(condition)
#endif


#define CHECK(condition) assert(condition)


#if defined(__GNUC__)
    #define BLVM_UNREACHABLE(str) assert(false && str)
#else
    #define BLVM_UNREACHABLE(str) assert(false && str)
#endif

#if defined(__GNUC__)
    #define BLVM_CRASH() __builtin_trap()
#else
    #define BLVM_CRASH() (*reinterpret_cast<volatile int*>(0)=0)
#endif

namespace blvm {
namespace base {

}
}

#endif // _BLVM_BASE_ERROR_HANDLING_HPP
