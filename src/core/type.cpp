#include "type.hpp"
#include "blvm_context.hpp"

using namespace blvm::bitcode;

namespace blvm {
namespace core {

    TypeRef Type::ObtainSimpleType(BLVMContext& context, bitcode::TypeCodes type_code) {
        switch (type_code) {
            case TypeCodes::kVoid:
                return context.type_void_;
            case TypeCodes::kHalf:
                return context.type_half_;
            case TypeCodes::kFloat:
                return context.type_float_;
            case TypeCodes::kDouble:
                return context.type_double_;
            case TypeCodes::kX86_FP80:
                return context.type_x86_fp80_;
            case TypeCodes::kX86_MMX:
                return context.type_x86_mmx;
            case TypeCodes::kFP128:
                return context.type_fp128_;
            case TypeCodes::kPPC_FP128:
                return context.type_ppc_fp128_;
            case TypeCodes::kLabel:
                return context.type_label_;
            case TypeCodes::kMetadata:
                return context.type_metadata_;
            default:
                return nullptr;
        }
    }

    bool IntegerType::IsCommonBitwidth(uint32_t bit_width) {
        if (bit_width == 1 ||bit_width == 8 || bit_width == 16 || bit_width == 32 || bit_width == 64) {
            return true;
        }
        return false;
    }

    TypeRef IntegerType::ObtainIntegerType(BLVMContext& context, uint32_t bit_width) {
        switch (bit_width) {
            case 1:
                return context.type_int1_;
            case 8:
                return context.type_int8_;
            case 16:
                return context.type_int16_;
            case 32:
                return context.type_int32_;
            case 64:
                return context.type_int64_;
        }
        return nullptr;
    }

    bool PointerType::IsValidTargetType(bitcode::TypeCodes type_code) {
        return type_code != TypeCodes::kVoid &&
               type_code != TypeCodes::kLabel &&
               type_code != TypeCodes::kMetadata;
    }

    bool ArrayType::IsValidElementType(bitcode::TypeCodes type_code) {
        return type_code != TypeCodes::kVoid &&
               type_code != TypeCodes::kLabel &&
               type_code != TypeCodes::kMetadata &&
               type_code != TypeCodes::kFunction &&
               type_code != TypeCodes::kFunction_Old;
    }

    bool VectorType::IsValidElementType(bitcode::TypeCodes type_code) {
        return type_code == TypeCodes::kInteger ||
               type_code == TypeCodes::kHalf ||
               type_code == TypeCodes::kFloat ||
               type_code == TypeCodes::kDouble ||
               type_code == TypeCodes::kPointer;
    }

    bool StructType::IsValidMemberType(bitcode::TypeCodes type_code) {
        return ArrayType::IsValidElementType(type_code);
    }

    bool FunctionType::IsValidArgumentType(TypeCodes type_code) {
        return type_code != TypeCodes::kVoid &&
               type_code != TypeCodes::kFunction &&
               type_code != TypeCodes::kFunction_Old;
    }

}
}
