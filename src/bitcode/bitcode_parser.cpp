#include "bitcode_parser.hpp"
#include "bitcode_reader.hpp"
#include "parsing_context.hpp"
#include "parsing_exception.hpp"
#include "bitcode_llvm.hpp"
#include "../core/module.hpp"
#include "../core/type.hpp"

namespace blvm {
namespace bitcode {

    BitcodeParser::BitcodeParser(core::BLVMContext& context, ParsingContext& parsing_context,
                                 BitcodeReader& reader, core::Module& target_module) :
            context_(context), parsing_context_(parsing_context), reader_(reader), module_(target_module) {

    }

    void BitcodeParser::Parse() {
        ValidateHeader();
        ParseModuleBlock();
    }

    std::string BitcodeParser::ConvertOpsToString(const std::vector<uint64_t> &ops) {
        std::string result;

        for (auto iter = ops.begin(); iter != ops.end(); ++iter)
            result.push_back((char)*iter);

        return std::move(result);
    }

    void BitcodeParser::ConvertOpsToString(const std::vector<uint64_t>& ops, std::string& out_string) {
        for (auto iter = ops.begin(); iter != ops.end(); ++iter)
            out_string.push_back((char)*iter);
    }

    void BitcodeParser::ValidateHeader() {
        if (reader_.Read(8) != 'B' ||
            reader_.Read(8) != 'C' ||
            reader_.Read(4) != 0x0 ||
            reader_.Read(4) != 0xC ||
            reader_.Read(4) != 0xE ||
            reader_.Read(4) != 0xD) {
            throw ParserException(ParserError::kDataError);
        }
    }

    void BitcodeParser::ParseModuleBlock() {
        using Entry = BitcodeReader::Entry;

        Entry entry = reader_.ReadNextEntry();
        if (entry.kind != Entry::Kind::kSubBlock || entry.id != BlockIds::kModuleBlock)
            throw ParserException(ParserError::kDataError);

        reader_.EnterSubBlock(BlockIds::kModuleBlock);

        while (true) {
            entry = reader_.ReadNextEntry();

            if (entry.kind == Entry::Kind::kError) {
                throw ParserException(ParserError::kDataError);
            } else if (entry.kind == Entry::Kind::kEndBlock) {
                reader_.ReadBlockEnd();
                break;
            } else if (entry.kind == Entry::Kind::kSubBlock) {
                switch (entry.id) {
                    case StandardBlockIds::kBlockInfo:
                        ParseBlockInfoBlock();
                        break;
                    case BlockIds::kModuleBlock:
                        throw ParserException(ParserError::kDataError);
                    case BlockIds::kTypeBlock:
                        ParseTypeBlock();
                        break;
                    case BlockIds::kParamattrBlock:
                    case BlockIds::kParamattrGroupBlock:
                    case BlockIds::kConstantBlock:
                    case BlockIds::kMetadataBlock:
                    case BlockIds::kMetadataAttachment:
                    case BlockIds::kValueSymtabBlock:
                    case BlockIds::kFunctionBlock:
                    case BlockIds::kUselistBlock:
                    default:
                        reader_.SkipSubBlock(entry.id);
                }
            } else {    // record
                std::vector<uint64_t> ops;
                uint32_t record_code = reader_.ReadRecord(entry.id, ops);

                switch (record_code) {
                    case ModuleCodes::kVersion:
                        if (ops.empty())
                            throw ParserException(ParserError::kDataError);
                        if (ops[0] != 1)    // current we only support module version 1
                            throw ParserException(ParserError::kNotSupproted);
                        module_.module_version = static_cast<int>(ops[0]);
                        break;
                    case ModuleCodes::kTriple:
                        if (!ops.empty()) {
                            module_.target_triple = std::move(ConvertOpsToString(ops));
                        }
                        break;
                    case ModuleCodes::kDataLayout:
                        if (!ops.empty()) {
                            module_.target_datalayout = std::move(ConvertOpsToString(ops));
                        }
                        break;
                    case ModuleCodes::kAsm:
                        throw ParserException(ParserError::kNotSupproted);
                        break;
                    case ModuleCodes::kSectionName:
                        if (!ops.empty()) {
                            std::string section_name = ConvertOpsToString(ops);
                            module_.section_name_table.push_back(std::move(section_name));
                        }
                        break;
                    case ModuleCodes::kDepLib:
                        // unused, skip
                        break;
                    case ModuleCodes::kGlobalVar:

                        break;
                    case ModuleCodes::kFunction:

                        break;
                    case ModuleCodes::kAlias:

                        break;
                    case ModuleCodes::kPurgeVals:

                        break;
                    case ModuleCodes::kGcName:
                        if (!ops.empty()) {
                            std::string gc_name = ConvertOpsToString(ops);
                            module_.gc_name_table.push_back(std::move(gc_name));
                        }
                        break;
                    case ModuleCodes::kComdat:

                        break;
                }
            }


        }
    }

    void BitcodeParser::ParseBlockInfoBlock() {
        using Entry = BitcodeReader::Entry;

        if (parsing_context_.HasBlockInfos()) {
            reader_.SkipSubBlock(StandardBlockIds::kBlockInfo);
            return;
        }

        reader_.EnterSubBlock(StandardBlockIds::kBlockInfo);

        BlockInfo* target_blockinfo = nullptr;
        std::vector<uint64_t> ops;

        while (true) {
            Entry entry = reader_.ReadNextEntry(BitcodeReader::kDontProcessAbbrevDefinitions);

            switch (entry.kind) {
                case Entry::Kind::kError:
                    throw ParserException(ParserError::kDataError);
                case Entry::Kind::kSubBlock:
                    reader_.SkipSubBlock(entry.id);  // unknown content, skip it
                    continue;
                case Entry::Kind::kEndBlock:
                    reader_.ReadBlockEnd();
                    return;
                case Entry::Kind::kRecord:
                    break;
            }

            if (entry.id == BuiltinAbbrevId::kDefineAbbrev) {
                if (target_blockinfo == nullptr)
                    throw ParserException(ParserError::kDataError);

                AbbrevRef abbrev = reader_.ReadAbbrevDefinition();
                target_blockinfo->abbrevs.push_back(std::move(abbrev));
                continue;
            }

            ops.clear();
            uint32_t code = reader_.ReadRecord(entry.id, ops);

            switch (static_cast<BlockInfoCodes>(code)) {
                case BlockInfoCodes::kSetBID:
                    if (ops.empty())
                        throw ParserException(ParserError::kDataError);
                    target_blockinfo = parsing_context_.GetOrCreateBlockInfo((uint32_t)ops[0]);
                    break;
                case BlockInfoCodes::kBlockName:
                    if (target_blockinfo == nullptr)
                        throw ParserException(ParserError::kDataError);
                    target_blockinfo->block_name = std::move(ConvertOpsToString(ops));
                    break;
                case BlockInfoCodes::kSetRecordName: {
                    if (target_blockinfo == nullptr)
                        throw ParserException(ParserError::kDataError);
                    std::string record_name;
                    if (ops.size() > 1) {
                        for (int i = 1; i < ops.size(); i++)
                            record_name.push_back((char)ops[i]);
                    }
                    target_blockinfo->record_names.push_back(std::make_pair((uint32_t)ops[0], std::move(record_name)));
                    break;
                }
            }
        }
    }

    void BitcodeParser::ParseTypeBlock() {
        using Entry = BitcodeReader::Entry;
        using namespace blvm::core;

        reader_.EnterSubBlock(BlockIds::kTypeBlock);

        std::vector<uint64_t> ops;

        // Parse TYPE_CODE_NUMENTRY
        uint32_t code = reader_.ReadRecord(reader_.ReadNextEntry().id, ops);
        if (code != static_cast<uint32_t>(TypeCodes::kNumEntry) || ops.size() != 1)
            throw ParserException(ParserError::kDataError);

        size_t entries = static_cast<size_t>(ops[0]);

        for (size_t i = 0; i <= entries; i++) {
            Entry entry = reader_.ReadNextEntry();
            if (entry.kind != Entry::Kind::kRecord)
                throw ParserException(ParserError::kDataError);

            ops.clear();
            TypeCodes type_code = static_cast<TypeCodes>(reader_.ReadRecord(entry.id, ops));

            TypeRef result_type;

            switch (type_code) {
                case TypeCodes::kNumEntry:
                    throw ParserException(ParserError::kDataError);
                case TypeCodes::kVoid:
                case TypeCodes::kHalf:
                case TypeCodes::kFloat:
                case TypeCodes::kDouble:
                case TypeCodes::kX86_FP80:
                case TypeCodes::kX86_MMX:
                case TypeCodes::kFP128:
                case TypeCodes::kPPC_FP128:
                case TypeCodes::kLabel:
                case TypeCodes::kMetadata:
                    result_type = Type::ObtainSimpleType(context_, type_code);
                    break;
                case TypeCodes::kInteger: {
                    uint32_t bit_width = static_cast<uint32_t>(ops[0]);
                    if (IntegerType::IsCommonBitwidth(bit_width)) {
                        result_type = IntegerType::ObtainIntegerType(context_, bit_width);
                    } else {
                        result_type = new IntegerType(bit_width);
                    }
                    break;
                }
                case TypeCodes::kPointer:
                    if (ops.size() >= 1) {
                        uint32_t address_space = 0;
                        if (ops.size() >= 2)
                            address_space = static_cast<uint32_t>(ops[1]);
                        result_type = new PointerType((uint32_t)ops[0], address_space);
                    } else {
                        throw ParserException(ParserError::kDataNotEnough);
                    }
                    break;
                case TypeCodes::kArray:
                    if (ops.size() >= 2)
                        result_type = new ArrayType((uint32_t) ops[0], (uint32_t) ops[1]);
                    else
                        throw ParserException(ParserError::kDataNotEnough);
                    break;
                case TypeCodes::kVector:
                    if (ops.size() >= 2)
                        result_type = new VectorType((uint32_t)ops[0], (uint32_t)ops[1]);
                    else
                        throw ParserException(ParserError::kDataNotEnough);
                    break;
                case TypeCodes::kOpaque:
                case TypeCodes::kStruct_ANON:
                case TypeCodes::kStruct_NAME:  // TODO: This isn't a type declaration
                case TypeCodes::kStruct_NAMED:
                case TypeCodes::kFunction_Old:
                case TypeCodes::kFunction:
                    break;
                default:
                    throw ParserException(ParserError::kNotSupproted);
            }
            module_.type_table.push_back(std::move(result_type));
        }
        reader_.ReadBlockEnd();  // TODO: ReadBlockEnd should be after ReadNextEntry, not guess
    }

}
}