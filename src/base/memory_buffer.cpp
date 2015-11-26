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
        assert(allocate_size > 0);

        begin_ = new uint8_t[allocate_size];
        end_ = begin_ + allocate_size;
        allocate_internal_ = true;
    }

    MemoryBuffer::~MemoryBuffer() {
        if (allocate_internal_) {
            delete [] begin_;
            begin_ = end_ = nullptr;
        }
    }

    int MemoryBuffer::ReadBytes(uint8_t* dest, int begin_index, int length) const {
        assert(begin_index >= 0 && length > 0);
        uint8_t* src_begin = begin_ + begin_index;
        uint8_t* expect_end = src_begin + length;
        if (expect_end > end_)
            expect_end = end_;
        int copy_length = (int)(expect_end - src_begin);

        if (copy_length == sizeof(size_t)) {
            *(size_t*)dest = *(size_t*)src_begin;
        } else {
            memcpy(dest, src_begin, (size_t)copy_length);
        }
        return copy_length;
    }

    const uint8_t* MemoryBuffer::GetAddressAt(int index) const {
        assert(index >= 0);
        uint8_t* addr = begin_ + index;
        if (addr >= end_)
            return nullptr;
        return addr;
    }

}
}