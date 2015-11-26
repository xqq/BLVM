#include <cstdio>
#include "base/ref_base.hpp"
#include "base/ref_ptr.hpp"
#include "base/error_handling.hpp"
#include "bitcode/bitcode_base.hpp"
#include "bitcode/bitcode_llvm.hpp"
#include "core/blvm_context.hpp"
#include "core/module.hpp"

namespace blvm {

    class FooClass : public base::RefBase {
    public:
        FooClass() = default;

        virtual ~FooClass() {
            printf("destructor for FooClass\n");
        }

        int Fuck(int i) {
            return dummy_integer_ + i;
        }

    private:
        int dummy_integer_ = 12450;
    };

    int dummy_func(void *ptr) {
        base::RefPtr<FooClass> pfoo = new FooClass;
        return pfoo->Fuck(*reinterpret_cast<int*>(ptr));
    }

}