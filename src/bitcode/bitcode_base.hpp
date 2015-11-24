#ifndef _BLVM_BITCODE_BITCODE_BASE_HPP
#define _BLVM_BITCODE_BITCODE_BASE_HPP

#include <cstdint>
#include <vector>
#include <string>
#include "../base/ref_counted.hpp"
#include "../base/ref_ptr.hpp"

namespace blvm {
namespace bitcode {

    enum BuiltinAbbrevId {
        kEndBlock = 0,
        kEnterSubBlock = 1,
        kDefineAbbrev = 2,
        kUnabbrevRecord = 3,

        kFirstApplicationAbbrev = 4
    };

    enum StandardBlockIds {
        kBlockInfo = 0,

        kFirstApplicationBlockId = 8
    };

    enum CommonBitWidth {
        kBlockIdWidth = 8,
        kNewAbbrevIdWidth = 4,
        kBlockSizeWidth = 32
    };

    enum class BlockInfoCodes {
        kSetBID = 1,
        kBlockName = 2,
        kSetRecordName = 3
    };


    class Abbreviation;
    typedef base::RefPtr<Abbreviation> AbbrevRef;

    struct BlockInfo {
        int block_id;
        std::vector<AbbrevRef> abbrevs;
        std::string block_name;

        std::vector<std::pair<int, std::string>> record_names;
    };

    struct Block {
        int block_id;
        int block_size;
        int abbrevid_length;
        std::vector<AbbrevRef> abbrevs;

        explicit Block(int abbrevid_len) : block_id(0), block_size(0), abbrevid_length(abbrevid_len) {}

        explicit Block(Block&& rhs) {
            operator=(std::move(rhs));
        }

        Block& operator=(Block&& rhs) {
            if (this != &rhs) {
                block_id = rhs.block_id;  rhs.block_id = 0;
                block_size = rhs.block_size;  rhs.block_size = 0;
                abbrevid_length = rhs.abbrevid_length;  rhs.abbrevid_length = 0;
                abbrevs = std::move(rhs.abbrevs);
            }
            return *this;
        }
    };

    class AbbrevOp;

    class Abbreviation : public base::RefCounted<Abbreviation> {
    public:
        Abbreviation() {
            operand_list_.reserve(32);
        }

        ~Abbreviation() = default;

        size_t GetOperandCount() const {
            return operand_list_.size();
        }

        const AbbrevOp& GetOperandInfoAt(size_t n) const {
            return operand_list_[n];
        }

        void AddOperand(const AbbrevOp& op_info) {
            operand_list_.push_back(op_info);
        }

        void AddOperand(AbbrevOp&& op_info) {
            operand_list_.push_back(std::move(op_info));
        }
    private:
        std::vector<AbbrevOp> operand_list_;
    };

    class AbbrevOp {
    public:
        enum class Encoding : int {
            kNull = 0,

            kFixed = 1,
            kVBR = 2,
            kArray = 3,
            kChar6 = 4,
            kBlob = 5
        };
    public:
        AbbrevOp() : value_(0), is_literal_(false), encoding_(Encoding::kNull) {}
        explicit AbbrevOp(uint64_t value) : value_(value), is_literal_(true), encoding_(Encoding::kNull) {}
        explicit AbbrevOp(Encoding encoding, uint64_t value = 0) : value_(value), is_literal_(false), encoding_(encoding) {}

        bool IsLiteral() const {
            return is_literal_;
        }

        uint64_t GetLiteralValue() const {
            assert(is_literal_);
            return value_;
        }

        Encoding GetEncoding() const {
            return encoding_;
        }

        int64_t GetEncodingExtraData() const {
            return value_;
        }
    public:
        static bool HasEncodingData(Encoding encoding);
        static bool CanEncodeInChar6(char c);
        static unsigned EncodeChar6(char c);
        static char DecodeChar6(unsigned input);
    private:
        uint64_t value_;
        bool is_literal_;
        Encoding encoding_;
    };

    bool AbbrevOp::HasEncodingData(Encoding encoding) {
        switch (encoding) {
            case Encoding::kFixed:
            case Encoding::kVBR:
                return true;
            case Encoding::kArray:
            case Encoding::kChar6:
            case Encoding::kBlob:
                return false;
            default:
                assert(false);
        }
        return false;
    }

    bool AbbrevOp::CanEncodeInChar6(char c) {
        if (c >= 'a' && c <= 'z')
            return true;
        if (c >= 'A' && c <= 'Z')
            return true;
        if (c >= '0' && c <= '9')
            return true;
        if (c == '.' || c == '_')
            return true;
        return false;
    }

    unsigned AbbrevOp::EncodeChar6(char c) {
        if (c >= 'a' && c <= 'z')
            return c - 'a';
        if (c >= 'A' && c <= 'Z')
            return c - 'A' + 26;
        if (c >= '0' && c <= '9')
            return c - '0' + 26 + 26;
        if (c == '.')
            return 62;
        if (c == '_')
            return 63;
        assert(false);
        return 0;
    }

    char AbbrevOp::DecodeChar6(unsigned input) {
        if (input <= 25)
            return input + 'a';
        if (input <= 51)
            return input - 26 + 'A';
        if (input <= 61)
            return input - 52 + '0';
        if (input == 62)
            return '.';
        if (input == 63)
            return '_';
        assert(false);
        return '\0';
    }

}
}

#endif // _BLVM_BITCODE_BITCODE_BASE_HPP
