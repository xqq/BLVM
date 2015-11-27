#ifndef _BLVM_CORE_MODULE_HPP
#define _BLVM_CORE_MODULE_HPP

#include <string>
#include <vector>
#include "../base/ref_ptr.hpp"

namespace blvm {
namespace core {

    class Type;
    typedef base::RefPtr<Type> TypeRef;

    class Module {
    public:
        int module_version;
        std::string target_triple;
        std::string target_datalayout;
        std::vector<TypeRef> type_table;
    public:
        Module();
    private:
        TypeRef type_void_, type_half_, type_float_, type_double_;
        TypeRef type_x86_fp80_, type_x86_mmx, type_fp128_, type_ppc_fp128_;
        TypeRef type_label_, type_metadata_;

        TypeRef type_int1_, type_int8_, type_int16_, type_int32_, type_int64_;
    private:

    };

}
}

#endif // _BLVM_CORE_MODULE_HPP
