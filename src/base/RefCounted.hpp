#ifndef _BASE_RECOUNTED_HPP
#define _BASE_RECOUNTED_HPP

#include <cstdint>
#include "Atomic.hpp"
#include "Noncopyable.hpp"

namespace base {

    template <typename T>
    class RefCounted {
    protected:
        explicit RefCounted() : ref_count_(0) {}

        ~RefCounted() {
            ref_count_ = 0;
        }
    public:
        void AddRef() {
            AtomicIncrease(&ref_count_);
        }

        void Release() {
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
        explicit RefCountedNonAtomic() : ref_count_(0) {}

        ~RefCountedNonAtomic() {
            ref_count_ = 0;
        }
    public:
        void AddRef() {
            ++ref_count_;
        }

        void Release() {
            if (0 == --ref_count_)
                delete static_cast<const T*>(this);
        }
    private:
        DISALLOW_COPY_AND_ASSIGN(RefCountedNonAtomic<T>);
    private:
        mutable int32_t ref_count_;
    };


}


#endif // _BASE_RECOUNTED_HPP
