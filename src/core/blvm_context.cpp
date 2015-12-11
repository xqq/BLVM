#include "blvm_context.hpp"
#include "type.hpp"

using namespace blvm::bitcode;

namespace blvm {
namespace core {

    BLVMContext::BLVMContext() {
        type_void_ = new Type(TypeCodes::kVoid);
        type_half_ = new Type(TypeCodes::kHalf);
        type_float_ = new Type(TypeCodes::kFloat);
        type_double_ = new Type(TypeCodes::kDouble);

        type_x86_fp80_ = new Type(TypeCodes::kX86_FP80);
        type_x86_mmx = new Type(TypeCodes::kX86_MMX);
        type_fp128_ = new Type(TypeCodes::kFP128);
        type_ppc_fp128_ = new Type(TypeCodes::kPPC_FP128);
        type_label_ = new Type(TypeCodes::kLabel);
        type_metadata_ = new Type(TypeCodes::kMetadata);

        type_int1_ = new IntegerType(1);
        type_int8_ = new IntegerType(8);
        type_int16_ = new IntegerType(16);
        type_int32_ = new IntegerType(32);
        type_int64_ = new IntegerType(64);
    }

    BLVMContext::~BLVMContext() {

    }

}
}