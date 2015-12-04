#ifndef _BLVM_BASE_MEMORY_BUFFER_HPP
#define _BLVM_BASE_MEMORY_BUFFER_HPP

#include <cstdint>
#include "noncopyable.hpp"

namespace blvm {
namespace base {

    class MemoryBuffer {
    public:
        enum : int {
            kAllocatedMemory = 0,
            kAllocateInternally = 1
        };
    public:
        MemoryBuffer(int allocate_method, uint8_t* existing_address, size_t size);
        MemoryBuffer(int allocate_method, uint8_t* begin, uint8_t* end);
        MemoryBuffer(int allocate_method, size_t allocate_size);
        MemoryBuffer(MemoryBuffer&& rhs) = default;
        ~MemoryBuffer();

        MemoryBuffer& operator=(MemoryBuffer&& rhs) = default;

        size_t ReadBytes(uint8_t* dest, size_t begin_offset, size_t length) const;

        const uint8_t* GetAddressAt(size_t offset) const;

        const uint8_t* begin() const {
            return begin_;
        }

        const uint8_t* end() const {
            return end_;
        }

        size_t size() const {
            return static_cast<size_t>(end_ - begin_);
        }
    private:
        uint8_t* begin_;
        uint8_t* end_;
        bool allocate_internal_;

        DISALLOW_COPY_AND_ASSIGN(MemoryBuffer);
    };

}
}

#endif // _BLVM_BASE_MEMORY_BUFFER_HPP
