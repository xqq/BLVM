#ifndef _BLVM_CORE_CORE_FWD_HPP
#define _BLVM_CORE_CORE_FWD_HPP

#include "../base/ref_ptr.hpp"

namespace blvm {
namespace core {

    class BLVMContext;
    class Module;

    class Type;
    typedef base::RefPtr<Type> TypeRef;

}
}

#endif // _BLVM_CORE_CORE_FWD_HPP
