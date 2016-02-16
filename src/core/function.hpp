#ifndef _BLVM_CORE_FUNCTION_HPP
#define _BLVM_CORE_FUNCTION_HPP

#include "../base/ref_base.hpp"

namespace blvm {
namespace core {

    class Function : public base::RefBase {
    public:
        Function() = default;
        virtual ~Function() override = default;

    private:

    };

}
}

#endif // _BLVM_CORE_FUNCTION_HPP
