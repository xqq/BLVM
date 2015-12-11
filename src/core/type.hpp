#ifndef _BLVM_CORE_TYPE_HPP
#define _BLVM_CORE_TYPE_HPP

#include "../base/ref_base.hpp"
#include "../base/ref_ptr.hpp"
#include "../bitcode/bitcode_llvm.hpp"
#include "core_fwd.hpp"

namespace blvm {
namespace core {

    class Type;
    typedef base::RefPtr<Type> TypeRef;

    class Type : public base::RefBase {
    public:
        Type(bitcode::TypeCodes type_code) : type_code_(type_code) {}
        virtual ~Type() = default;

        bitcode::TypeCodes GetTypeCode() const {
            return type_code_;
        }
    public:
        static TypeRef ObtainSimpleType(BLVMContext& context, bitcode::TypeCodes type_code);
    private:
        bitcode::TypeCodes type_code_;
    };


    class IntegerType : public Type {
    public:
        explicit IntegerType(uint32_t bit_width) :
                Type(bitcode::TypeCodes::kInteger), bit_width_(bit_width) {}
        virtual ~IntegerType() override = default;

        uint32_t GetBitWidth() const {
            return bit_width_;
        }
    public:
        static bool IsCommonBitwidth(uint32_t bit_width);
        static TypeRef ObtainIntegerType(BLVMContext& context, uint32_t bit_width);
    private:
        uint32_t bit_width_;
    };


    class PointerType : public Type {
    public:
        PointerType(uint32_t pointee_type_index, uint32_t address_space = 0) :
                Type(bitcode::TypeCodes::kPointer),
                pointee_type_index_(pointee_type_index), address_space_(address_space) {}
        virtual ~PointerType() override = default;
    private:
        uint32_t pointee_type_index_;
        uint32_t address_space_;
    };


    class ArrayType : public Type {
    public:
        ArrayType(uint32_t element_count, uint32_t element_type_index) :
                Type(bitcode::TypeCodes::kArray),
                element_count_(element_count), element_type_index_(element_type_index) {}
        virtual ~ArrayType() override = default;
    private:
        uint32_t element_count_;
        uint32_t element_type_index_;
    };


    class VectorType : public Type {
    public:
        VectorType(uint32_t element_count, uint32_t element_type_index) :
                Type(bitcode::TypeCodes::kArray),
                element_count_(element_count), element_type_index_(element_type_index) {}
        virtual ~VectorType() override = default;
    private:
        uint32_t element_count_;
        uint32_t element_type_index_;
    };


}
}

#endif // _BLVM_CORE_TYPE_HPP
