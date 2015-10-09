#ifndef _BASE_REFPTR_HPP
#define _BASE_REFPTR_HPP

#include <cassert>

namespace base {

    template <typename T>
    class RefPtr {
    public:
        RefPtr() : ptr_(nullptr) {}

        ~RefPtr() {
            if (ptr_) {
                ptr_->Release();
                ptr_ = nullptr;
            }
        }

        RefPtr(T* p) : ptr_(p) {
            if (ptr_)
                ptr_->AddRef();
        }

        RefPtr(const RefPtr<T>& r) : ptr_(r.ptr_) {
            if (ptr_)
                ptr_->AddRef();
        }

        template <typename U>
        RefPtr(const RefPtr<U>& r) : ptr_(r.Get()) {
            if (ptr_)
                ptr_->AddRef();
        }

        template <typename U>
        RefPtr(RefPtr<U>&& r) : ptr_(r.Get()) {
            r.ptr_ = nullptr;
        }

        T* Get() const {
            return ptr_;
        }

        T** GetAddressOf() const {
            return &ptr_;
        }

        T& operator*() const {
            assert(ptr_ != nullptr);
            return *ptr_;
        }

        T* operator->() const {
            assert(ptr_ != nullptr);
            return ptr_;
        }

        T** operator&() {
            if (ptr_) {
                ptr_->Release();
                ptr_ = nullptr;
            }
            return &ptr_;
        }

        RefPtr<T>& operator=(T* p) {
            if (p)
                p->AddRef();
            T* old = ptr_;
            ptr_ = p;
            if (old)
                old->Release();
            return *this;
        }

        RefPtr<T>& operator=(const RefPtr<T>& r) {
            return operator=(r.ptr_);
        }

        template <typename U>
        RefPtr<T> operator=(const RefPtr<U>& r) {
            return operator=(r.Get());
        }

        template <typename U>
        bool operator==(const RefPtr<U>& rhs) const {
            return ptr_ == rhs.Get();
        }

        template <typename U>
        bool operator!=(const RefPtr<U>& rhs) const {
            return ptr_ != rhs.Get();
        }

        template <typename U>
        bool operator<(const RefPtr<U>& rhs) const {
            return ptr_ < rhs.Get();
        }

        void Reset(T* p) {
            operator=(p);
        }

        void Reset() {
            Reset(nullptr);
        }

        void Swap(T** pp) {
            T* old = ptr_;
            ptr_ = *pp;
            *pp = old;
        }

        void Swap(const RefPtr<T>& r) {
            Swap(&r.ptr_);
        }
    private:
        template <typename U>
        friend class RefPtr;

        typedef T* RefPtr::*Testable;
    public:
        operator Testable() const {
            return ptr_ ? &RefPtr::ptr_ : nullptr;
        }
    private:
        T* ptr_;
    };

}

#endif // _BASE_REFPTR_HPP
