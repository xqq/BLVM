#include "bitcode_reader.hpp"
#include "../base/error_handling.hpp"
#include "parsing_exception.hpp"

namespace blvm {
namespace bitcode {

    BitcodeReader::BitcodeReader(core::BLVMContext& context, core::Module& module, base::MemoryBuffer& bitcode_buffer) :
            context_(context), module_(module), buffer_(bitcode_buffer),
            buffer_index_(0), current_word_(0), current_word_bits_left_(0), current_block_(2) {
        FillCurrentWord();
    }

    BitcodeReader::~BitcodeReader() {

    }

    BitcodeReader::Entry BitcodeReader::ReadNextEntry() {
        while (true) {
            word_t word = Read(current_block_.abbrevid_length);
            switch (word) {
                case BuiltinAbbrevId::kEndBlock:
                    return Entry::MakeEndBlock();
                case BuiltinAbbrevId::kEnterSubBlock:
                    return Entry::MakeSubBlock(ReadSubBlockId());
                case BuiltinAbbrevId::kDefineAbbrev:
                    ReadAbbrevRecord();
                    continue;
                case BuiltinAbbrevId::kUnabbrevRecord:
                default:
                    return Entry::MakeRecord((uint32_t)word);
            }
        }
    }

    void BitcodeReader::FillCurrentWord() {
        int buffer_bytes_left = (int)(buffer_.size() - buffer_index_);
        if (buffer_bytes_left <= 0)
            throw ReaderException(ReaderError::kEof);

        size_t expect_bytes = buffer_bytes_left >= sizeof(word_t) ? sizeof(word_t) : (size_t)buffer_bytes_left;
        size_t bytes_read = buffer_.ReadBytes(reinterpret_cast<uint8_t*>(&current_word_), buffer_index_, expect_bytes);
        if (bytes_read != expect_bytes)
            throw ReaderException(ReaderError::kDataNotEnough);

        buffer_index_ += bytes_read;
        current_word_bits_left_ = bytes_read * 8;
    }

    BitcodeReader::word_t BitcodeReader::Read(uint32_t bits) {
        static const size_t bits_per_word = sizeof(word_t) * 8;
        DCHECK(bits <= bits_per_word);

        if (bits <= current_word_bits_left_) {
            word_t result = current_word_ & (~word_t(0) >> (bits_per_word - bits));
            current_word_ >>= bits;
            current_word_bits_left_ -= bits;
            return result;
        }

        word_t result = current_word_bits_left_ ? current_word_ : 0;
        size_t bits_need_left = bits - current_word_bits_left_;

        FillCurrentWord();
        if (current_word_bits_left_ < bits_need_left)
            throw ReaderException(ReaderError::kDataNotEnough);

        word_t result2 = current_word_ & (~word_t(0) >> (bits_per_word - bits_need_left));
        current_word_ >>= bits_need_left;
        current_word_bits_left_ -= bits_need_left;

        result |= result2 << (bits - bits_need_left);
        return result;
    }

    uint32_t BitcodeReader::ReadVBR(uint32_t bits) {
        uint32_t piece = (uint32_t)Read(bits);
        if ((piece & (1u << (bits - 1))) == 0)
            return piece;

        uint32_t result = 0;
        uint32_t shift_left = 0;
        while (true) {
            result |= (piece & ((1u << (bits - 1)) - 1)) << shift_left;

            if ((piece & (1u << (bits - 1))) == 0)
                return result;

            piece = (uint32_t)Read(bits);
            shift_left += bits - 1;
        }
    }

    uint64_t BitcodeReader::ReadVBR64(uint32_t bits) {
        uint32_t piece = (uint32_t)Read(bits);
        if ((piece & (1u << (bits - 1))) == 0)
            return (uint64_t)piece;

        uint64_t result = 0;
        uint32_t shift_left = 0;
        while (true) {
            result |= uint64_t((piece & ((1u << (bits - 1)) - 1))) << shift_left;

            if ((piece & (1u << (bits - 1))) == 0)
                return result;

            piece = (uint32_t)Read(bits);
            shift_left += bits - 1;
        }
    }

    size_t BitcodeReader::GetCurrentBitPos() {
        return buffer_index_ * 8 - current_word_bits_left_;
    }

    bool BitcodeReader::IsValidBytePos(size_t byte_pos) {
        return byte_pos < buffer_.size();
    }

    void BitcodeReader::SeekToBitPos(size_t bit_pos) {
        size_t target_byte = bit_pos / 8;
        uint32_t target_byte_remain_bits = (uint32_t)(bit_pos % 8);

        if (target_byte >= buffer_.size())
            throw ReaderException(ReaderError::kEof);

        buffer_index_ = target_byte;
        current_word_bits_left_ = 0;

        if (target_byte_remain_bits != 0) {
            Read(target_byte_remain_bits);
        }
    }

    void BitcodeReader::SkipTo32bitsBoundary() {
        if (sizeof(word_t) > 4 && current_word_bits_left_ >= 32) {
            current_word_ >>= (current_word_bits_left_ - 32);
            current_word_bits_left_ = 32;
        } else {
            current_word_ = 0;
            current_word_bits_left_ = 0;
        }
    }

    uint32_t BitcodeReader::ReadSubBlockId() {
        return ReadVBR(CommonBitWidth::kBlockIdWidth);
    }

    // Having read ENTER_SUBBLOCK abbrevid, and the blockid(vbr8) operand
    void BitcodeReader::EnterSubBlock(uint32_t block_id) {
        Block block;
        block.block_id = block_id;
        block.abbrevid_length = ReadVBR(CommonBitWidth::kNewAbbrevIdWidth);
        SkipTo32bitsBoundary();
        block.block_size = (uint32_t)Read(CommonBitWidth::kBlockSizeWidth);

        if (block.block_size >= buffer_.size())
            throw ReaderException(ReaderError::kDataError);

        block_stack_.push(std::move(current_block_));
        current_block_ = std::move(block);
    }

    // Having read the ENTER_SUBBLOCK abbrevid, and the blockid(vbr8) operand
    void BitcodeReader::SkipSubBlock(uint32_t block_id) {
        ReadVBR(CommonBitWidth::kNewAbbrevIdWidth);
        SkipTo32bitsBoundary();
        word_t block_size_bytes = Read(CommonBitWidth::kBlockSizeWidth);

        if (block_size_bytes >= buffer_.size())
            throw ReaderException(ReaderError::kDataError);

        SeekToBitPos(GetCurrentBitPos() + block_size_bytes * 4 * 8);
    }

    // Having read the END_BLOCK abbrevid
    void BitcodeReader::ReadBlockEnd() {
        SkipTo32bitsBoundary();
        PopBlockScope();
    }

    void BitcodeReader::PopBlockScope() {
        if (block_stack_.empty())
            throw ReaderException(ReaderError::kScopeMismatch);

        current_block_ = std::move(block_stack_.top());
        block_stack_.pop();
    }

    // Having read the DEFINE_ABBREV abbrevid
    void BitcodeReader::ReadAbbrevRecord() {
        base::RefPtr<Abbreviation> abbrev = new Abbreviation();

        int numops = ReadVBR(5);
        for (int i = 0; i < numops; i++) {
            bool is_literal = (Read(1) == 1);
            if (is_literal) {
                abbrev->AddOperand(std::move(AbbrevOp(ReadVBR64(8))));
                continue;
            }

            int e = (int)Read(3);
            if (e <= 0 || e > 5)
                throw ReaderException(ReaderError::kDataError);

            AbbrevOp::Encoding encoding = static_cast<AbbrevOp::Encoding>(e);
            uint64_t value = 0;
            if (AbbrevOp::HasEncodingData(encoding)) {
                value = ReadVBR64(5);
                if ((encoding == AbbrevOp::Encoding::kFixed || encoding == AbbrevOp::Encoding::kVBR) && value == 0) {
                    abbrev->AddOperand(std::move(AbbrevOp(0)));
                    continue;
                }
            }
            abbrev->AddOperand(std::move(AbbrevOp(encoding, value)));
        }
        current_block_.abbrevs.push_back(abbrev);
    }

    uint64_t BitcodeReader::ReadAbbrevOp(const AbbrevOp& op) {  // TODO
        DCHECK(!op.IsLiteral());

        switch (op.GetEncoding()) {
            case AbbrevOp::Encoding::kArray:
            case AbbrevOp::Encoding::kBlob:
                BLVM_UNREACHABLE("Unsupported encoding type");
            case AbbrevOp::Encoding::kFixed:
                return Read((uint32_t)op.GetEncodingExtraData());
            case AbbrevOp::Encoding::kVBR:
                return ReadVBR64((uint32_t)op.GetEncodingExtraData());
            case AbbrevOp::Encoding::kChar6:
                return (uint64_t)AbbrevOp::DecodeChar6((uint32_t)Read(6));
            default:
                break;
        }
        BLVM_UNREACHABLE("Invalid encoding type!");
        return 0;
    }

    AbbrevRef BitcodeReader::GetAbbreviationById(uint32_t abbrevid) {
        uint32_t abbrev_index = abbrevid - BuiltinAbbrevId::kFirstApplicationAbbrev;
        if (abbrev_index >= current_block_.abbrevs.size())
            throw ReaderException(ReaderError::kDataError);

        return current_block_.abbrevs[abbrev_index];
    }

    uint32_t BitcodeReader::ReadRecord(uint32_t abbrevid, std::vector<uint32_t>& out_ops) {
        if (abbrevid == BuiltinAbbrevId::kUnabbrevRecord) {
            uint32_t code = ReadVBR(6);
            uint32_t numops = ReadVBR(6);
            for (uint32_t i = 0; i < numops; i++)
                out_ops.push_back(ReadVBR(6));
            return code;
        }

        base::RefPtr<Abbreviation> abbrev = GetAbbreviationById(abbrevid);

        if (abbrev->GetOperandCount() == 0)
            throw ReaderException(ReaderError::kDataError);

        const AbbrevOp& code_op = abbrev->GetOperandInfoAt(abbrevid);
        uint32_t code;
        if (code_op.IsLiteral()) {
            code = (uint32_t)code_op.GetLiteralValue();
        } else {
            AbbrevOp::Encoding encoding = code_op.GetEncoding();
            if (encoding == AbbrevOp::Encoding::kArray || encoding == AbbrevOp::Encoding::kBlob)
                throw ReaderException(ReaderError::kDataError);
            code = (uint32_t)ReadAbbrevOp(code_op);
        }

        size_t op_count = abbrev->GetOperandCount();
        for (size_t i = 1; i < op_count; i++) {
            const AbbrevOp& op = abbrev->GetOperandInfoAt(i);
            if (op.IsLiteral()) {
                out_ops.push_back((uint32_t)op.GetLiteralValue());
                continue;
            }

            AbbrevOp::Encoding encoding = op.GetEncoding();
            if (encoding != AbbrevOp::Encoding::kArray && encoding != AbbrevOp::Encoding::kBlob) {
                out_ops.push_back((uint32_t)ReadAbbrevOp(op));
                continue;
            }

            if (encoding == AbbrevOp::Encoding::kArray) {
                uint32_t element_count = ReadVBR(6);
                if (i + 2 != op_count)
                    throw ReaderException(ReaderError::kDataError);
                const AbbrevOp& element = abbrev->GetOperandInfoAt(++i);

                for (uint32_t j = 0; j < element_count; j++) {
                    out_ops.push_back((uint32_t)ReadAbbrevOp(element));
                }
                continue;
            }

            DCHECK(encoding == AbbrevOp::Encoding::kBlob);
            uint32_t element_count = ReadVBR(6);
            SkipTo32bitsBoundary();

            size_t current_bitpos = GetCurrentBitPos();
            size_t target_bitpos = current_bitpos + ((element_count + 3) & ~3) * 8;

            if (!IsValidBytePos(target_bitpos / 8)) {
                for (int k = 0; k < element_count; k++)
                    out_ops.push_back(0);
                buffer_index_ = buffer_.size();
                break;
            } else {
                const uint8_t* ptr = buffer_.GetAddressAt(current_bitpos / 8);

                for (int k = 0; k < element_count; k++) {
                    out_ops.push_back(*ptr++);
                }
            }

            SeekToBitPos(target_bitpos);
        }

        return code;
    }

}
}