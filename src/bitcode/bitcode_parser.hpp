#ifndef _BLVM_BITCODE_BITCODE_PARSER_HPP
#define _BLVM_BITCODE_BITCODE_PARSER_HPP

#include <vector>
#include <string>
#include "../base/noncopyable.hpp"
#include "../core/core_fwd.hpp"

namespace blvm {
namespace bitcode {

    class BitcodeReader;
    class ParsingContext;

    class BitcodeParser {
    public:
        BitcodeParser(core::BLVMContext& context, ParsingContext& parsing_context,
                      BitcodeReader& reader, core::Module& target_module);
        ~BitcodeParser() = default;
        void Parse();
    private:
        std::string ConvertOpsToString(const std::vector<uint64_t>& ops);
        void ConvertOpsToString(const std::vector<uint64_t>& ops, std::string& out_string);
        void ValidateHeader();
        void ParseModuleBlock();
        void ParseBlockInfoBlock();
        void ParseTypeBlock();
        void ParseParamattrBlock();
    private:
        core::BLVMContext& context_;
        ParsingContext& parsing_context_;
        BitcodeReader& reader_;
        core::Module& module_;

        DISALLOW_COPY_AND_ASSIGN(BitcodeParser);
    };

}
}

#endif // _BLVM_BITCODE_BITCODE_PARSER_HPP
