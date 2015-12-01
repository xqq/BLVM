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

    };

}
}

#endif // _BLVM_CORE_MODULE_HPP
