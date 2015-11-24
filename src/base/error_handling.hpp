#ifndef _BLVM_BASE_ERROR_HANDLING_HPP
#define _BLVM_BASE_ERROR_HANDLING_HPP

#include <cassert>

#ifndef NDEBUG
    #define DCHECK(x) assert(x)
#else
    #define DCHECK(x)
#endif


#ifndef NDEBUG
    #define CHECK(x) assert(x)
#else
    #undef NDEBUG
    #define CHECK(x) assert(x)
    #define NDEBUG
#endif


#ifndef NDEBUG
    #define UNREACHABLE(str) assert(false && str)
#else
    #undef NDEBUG
    #define UNREACHABLE(str) assert(false && str)
    #define NDEBUG
#endif

namespace blvm {
namespace base {

}
}

#endif // _BLVM_BASE_ERROR_HANDLING_HPP
