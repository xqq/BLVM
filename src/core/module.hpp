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
        std::vector<std::string> section_name_table;
        std::vector<std::string> gc_name_table;
    public:
        Module();
        ~Module();
        bool IsValidTypeIndex(uint32_t type_index) {
            return type_index < type_table.size();
        }
    private:

    };

}
}

#endif // _BLVM_CORE_MODULE_HPP
