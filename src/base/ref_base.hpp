#ifndef _BLVM_BASE_REFBASE_HPP
#define _BLVM_BASE_REFBASE_HPP

#include <cstdint>
#include "atomic.hpp"
#include "noncopyable.hpp"

namespace blvm {
namespace base {

    class RefBase {
    protected:
        RefBase() : ref_count_(0) {}

        virtual ~RefBase() {
            ref_count_ = 0;
        }
    public:
        void AddRef() {
            AtomicIncrease(&ref_count_);
        }

        void Release() {
            if (!AtomicDecrease(&ref_count_))
                delete this;
        }
    private:
        DISALLOW_COPY_AND_ASSIGN(RefBase);
    private:
        mutable int32_t ref_count_;
    };


    class RefBaseNonAtomic {
    protected:
        RefBaseNonAtomic() : ref_count_(0) {}

        virtual ~RefBaseNonAtomic() {
            ref_count_ = 0;
        }
    public:
        void AddRef() {
            ++ref_count_;
        }

        void Release() {
            if (0 == --ref_count_)
                delete this;
        }
    private:
        DISALLOW_COPY_AND_ASSIGN(RefBaseNonAtomic);
    private:
        mutable int32_t ref_count_;
    };


}
}

#endif // _BLVM_BASE_REFBASE_HPP
