#ifndef _BLVM_CORE_ATTRIBUTE_SET_HPP
#define _BLVM_CORE_ATTRIBUTE_SET_HPP

#include <cstdint>
#include <bitset>
#include "../base/ref_counted.hpp"
#include "../base/noncopyable.hpp"

namespace blvm {
namespace core {

    class AttributeSet : public base::RefCounted<AttributeSet> {
    public:
        enum AttrIndex : uint32_t {
            kReturnValueIndex = 0x0,
            kFunctionIndex = 0xFFFFFFFF
        };
    public:

    private:
        DISALLOW_COPY_AND_ASSIGN(AttributeSet);
    };

}
}

#endif // _BLVM_CORE_ATTRIBUTE_SET_HPP
