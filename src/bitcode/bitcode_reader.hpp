#ifndef _BLVM_BITCODE_BITCODE_READER_HPP
#define _BLVM_BITCODE_BITCODE_READER_HPP

#include <cstdint>
#include <stack>
#include <vector>
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
        word_t Read(uint32_t bits);
        uint32_t ReadVBR(uint32_t bits);
        uint64_t ReadVBR64(uint32_t bits);
        Entry ReadNextEntry();
        uint32_t ReadSubBlockId();
        void EnterSubBlock(uint32_t block_id);
        void SkipSubBlock(uint32_t block_id);
        void ReadBlockEnd();
        uint32_t ReadRecord(uint32_t abbrevid, std::vector<uint32_t>& out_ops);
    private:
        void FillCurrentWord();
        void SkipTo32bitsBoundary();
        size_t GetCurrentBitPos();
        bool IsValidBytePos(size_t byte_pos);
        void SeekToBitPos(size_t bit_pos);
        void PopBlockScope();
        void ReadAbbrevRecord();
        uint64_t ReadAbbrevOp(const AbbrevOp& op);
        AbbrevRef GetAbbreviationById(uint32_t abbrevid);
    private:
        core::BLVMContext& context_;
        core::Module& module_;

        base::MemoryBuffer& buffer_;
        size_t buffer_index_;

        word_t current_word_;
        size_t current_word_bits_left_;

        Block current_block_;
        std::stack<Block> block_stack_;

        DISALLOW_COPY_AND_ASSIGN(BitcodeReader);
    };


    struct BitcodeReader::Entry {
        enum class Kind {
            kError,
            kEndBlock,
            kSubBlock,
            kRecord
        };

        Kind kind;
        uint32_t id;

        explicit Entry(Kind _kind) : kind(_kind) {}
        Entry(Kind _kind, uint32_t _id) : kind(_kind), id(_id) {}

        static Entry MakeError() {
            return Entry(Kind::kError);
        }

        static Entry MakeEndBlock() {
            return Entry(Kind::kEndBlock);
        }

        static Entry MakeSubBlock(uint32_t id) {
            return Entry(Kind::kSubBlock, id);
        }

        static Entry MakeRecord(uint32_t abbrevid) {
            return Entry(Kind::kRecord, abbrevid);
        }
    };

}
}

#endif // _BLVM_BITCODE_BITCODE_READER_HPP
