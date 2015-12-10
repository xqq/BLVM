#include "blvm_context.hpp"
#include "type.hpp"

namespace blvm {
namespace core {

    BLVMContext::BLVMContext() {
        type_int8_ = new Type();
    }

    BLVMContext::~BLVMContext() {
        type_int8_.Reset();
    }

}
}