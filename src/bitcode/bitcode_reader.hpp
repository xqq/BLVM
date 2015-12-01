#ifndef _BLVM_BITCODE_BITCODE_READER_HPP
#define _BLVM_BITCODE_BITCODE_READER_HPP

#include <cstdint>
#include <stack>
#include "../base/noncopyable.hpp"
#include "../base/memory_buffer.hpp"
#include "bitcode_base.hpp"

namespace blvm {
namespace core {
    class BLVMContext;
    class Module;
}
}

namespace blvm {
namespace bitcode {

    class BitcodeReader {
    public:
        typedef size_t word_t;
        struct Entry;
    public:
        BitcodeReader(core::BLVMContext& context, core::Module& module, base::MemoryBuffer& bitcode_buffer);
        ~BitcodeReader();
        word_t Read(int bits);
        uint32_t ReadVBR(int bits);
        uint64_t ReadVBR64(int bits);
        Entry ReadNextEntry();
        bool EnterSubBlock(int block_id);
        bool SkipSubBlock(int block_id);
        bool ReadBlockEnd();

    private:
        void FillCurrentWord();
        void SkipTo32bitsBoundary();
        int GetCurrentBitPos();
        void SeekToBitPos(int bit_pos);
    private:
        core::BLVMContext& context_;
        core::Module& module_;

        base::MemoryBuffer& buffer_;
        int buffer_index_;

        word_t current_word_;
        int current_word_bits_left_;

        Block current_block_;
        std::stack<Block> block_stack_;

        DISALLOW_COPY_AND_ASSIGN(BitcodeReader);
    };

    struct BitcodeReader::Entry {

    };

}
}

#endif // _BLVM_BITCODE_BITCODE_READER_HPP
