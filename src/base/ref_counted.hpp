#ifndef _BLVM_BASE_RECOUNTED_HPP
#define _BLVM_BASE_RECOUNTED_HPP

#include <cstdint>
#include <cassert>
#include "atomic.hpp"
#include "noncopyable.hpp"

namespace blvm {
namespace base {

    template <typename T>
    class RefCounted {
    protected:
        RefCounted() : ref_count_(0) {}

        ~RefCounted() {
            assert(ref_count_ == 0);
        }
    public:
        void AddRef() const {
            AtomicIncrease(&ref_count_);
        }

        void Release() const {
            if (!AtomicDecrease(&ref_count_))
                delete static_cast<const T*>(this);
        }
    private:
        DISALLOW_COPY_AND_ASSIGN(RefCounted<T>);
    private:
        mutable int32_t ref_count_;
    };


    template <typename T>
    class RefCountedNonAtomic {
    protected:
        RefCountedNonAtomic() : ref_count_(0) {}

        ~RefCountedNonAtomic() {
            assert(ref_count_ == 0);
        }
    public:
        void AddRef() const {
            ++ref_count_;
        }

        void Release() const {
            if (0 == --ref_count_)
                delete static_cast<const T*>(this);
        }
    private:
        DISALLOW_COPY_AND_ASSIGN(RefCountedNonAtomic<T>);
    private:
        mutable int32_t ref_count_;
    };


}
}

#endif // _BLVM_BASE_RECOUNTED_HPP
