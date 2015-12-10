#include "memory_buffer.hpp"
#include <cassert>
#include <cstring>

namespace blvm {
namespace base {

    MemoryBuffer::MemoryBuffer(int allocate_method, uint8_t* existing_address, size_t size) :
            begin_(existing_address), end_(existing_address + size) {
        assert(allocate_method == kAllocatedMemory);
        allocate_internal_ = false;
    }

    MemoryBuffer::MemoryBuffer(int allocate_method, uint8_t* begin, uint8_t* end) :
            begin_(begin), end_(end) {
        assert(allocate_method == kAllocatedMemory);
        allocate_internal_ = false;
    }

    MemoryBuffer::MemoryBuffer(int allocate_method, size_t allocate_size) {
        assert(allocate_method == kAllocateInternally);

        begin_ = new uint8_t[allocate_size];
        memset(begin_, 0, allocate_size);
        end_ = begin_ + allocate_size;
        allocate_internal_ = true;
    }

    MemoryBuffer::~MemoryBuffer() {
        if (allocate_internal_) {
            delete [] begin_;
            begin_ = end_ = nullptr;
            allocate_internal_ = false;
        }
    }

    size_t MemoryBuffer::ReadBytes(uint8_t* dest, size_t begin_offset, size_t length) const {
        uint8_t* src_begin = begin_ + begin_offset;
        uint8_t* expect_end = src_begin + length;
        if (expect_end > end_)
            expect_end = end_;
        size_t copy_length = (size_t)(expect_end - src_begin);

        if (copy_length == sizeof(size_t)) {
            *(size_t*)dest = *(size_t*)src_begin;
        } else {
            memcpy(dest, src_begin, (size_t)copy_length);
        }
        return copy_length;
    }

    const uint8_t* MemoryBuffer::GetAddressAt(size_t offset) const {
        uint8_t* addr = begin_ + offset;
        if (addr >= end_)
            return nullptr;
        return addr;
    }

}
}