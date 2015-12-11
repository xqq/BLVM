#ifndef _BLVM_CORE_BLVM_CONTEXT_HPP
#define _BLVM_CORE_BLVM_CONTEXT_HPP

#include "../base/noncopyable.hpp"
#include "../base/ref_ptr.hpp"
#include "type.hpp"

namespace blvm {
namespace core {

    class BLVMContext {
    public:
        BLVMContext();
        ~BLVMContext();
    private:
        TypeRef type_void_, type_half_, type_float_, type_double_;
        TypeRef type_x86_fp80_, type_x86_mmx, type_fp128_, type_ppc_fp128_;
        TypeRef type_label_, type_metadata_;
        TypeRef type_int1_, type_int8_, type_int16_, type_int32_, type_int64_;

        friend class Type;
        friend class IntegerType;
        DISALLOW_COPY_AND_ASSIGN(BLVMContext);
    };

}
}

#endif // _BLVM_CORE_BLVM_CONTEXT_HPP
