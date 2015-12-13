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
            } else if (entry.kind == Entry::Kind::kRecord) {
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
                        for (size_t i = 1; i < ops.size(); i++)
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
        size_t entries = 0;
        std::string current_struct_typename;

        while (true) {
            Entry entry = reader_.ReadNextEntry();

            switch (entry.kind) {
                case Entry::Kind::kError:
                case Entry::Kind::kSubBlock:
                    throw ParserException(ParserError::kDataError);
                case Entry::Kind::kEndBlock:
                    if (entries != 0 && module_.type_table.size() != entries)
                        throw ParserException(ParserError::kDataError);
                    reader_.ReadBlockEnd();
                    return;
                case Entry::Kind::kRecord:
                    break;
            }

            ops.clear();
            TypeCodes type_code = static_cast<TypeCodes>(reader_.ReadRecord(entry.id, ops));

            TypeRef result_type;

            switch (type_code) {
                case TypeCodes::kNumEntry:
                    if (ops.size() < 1 || entries != 0)
                        throw ParserException(ParserError::kDataError);
                    entries = static_cast<size_t>(ops[0]);
                    continue;
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
                case TypeCodes::kPointer: {
                    if (ops.size() < 1)
                        throw ParserException(ParserError::kDataNotEnough);

                    uint32_t target_type_index = static_cast<uint32_t>(ops[0]);
                    if (!module_.IsValidTypeIndex(target_type_index))
                        throw ParserException(ParserError::kDataError);
                    if (!PointerType::IsValidTargetType(module_.type_table[target_type_index]->GetTypeCode()))
                        throw ParserException(ParserError::kNotSupproted);

                    uint32_t address_space = 0;
                    if (ops.size() >= 2)
                        address_space = static_cast<uint32_t>(ops[1]);
                    result_type = new PointerType(target_type_index, address_space);
                    break;
                }
                case TypeCodes::kArray: {
                    if (ops.size() < 2)
                        throw ParserException(ParserError::kDataNotEnough);

                    uint32_t element_type_index = static_cast<uint32_t>(ops[1]);
                    if (!module_.IsValidTypeIndex(element_type_index) ||
                        !ArrayType::IsValidElementType(module_.type_table[element_type_index]->GetTypeCode()))
                        throw ParserException(ParserError::kDataError);

                    result_type = new ArrayType((uint32_t)ops[0], element_type_index);
                    break;
                }
                case TypeCodes::kVector: {
                    if (ops.size() < 2)
                        throw ParserException(ParserError::kDataNotEnough);

                    if (ops[0] == 0)
                        throw ParserException(ParserError::kDataError);

                    uint32_t element_type_index = static_cast<uint32_t>(ops[1]);
                    if (!module_.IsValidTypeIndex(element_type_index) ||
                        !VectorType::IsValidElementType(module_.type_table[element_type_index]->GetTypeCode()))
                        throw ParserException(ParserError::kDataError);

                    result_type = new VectorType((uint32_t)ops[0], element_type_index);
                    break;
                }
                case TypeCodes::kOpaque:
                    if (ops.size() != 1)
                        throw ParserException(ParserError::kDataError);

                    result_type = new StructType(false, current_struct_typename);
                    current_struct_typename.clear();
                    break;
                case TypeCodes::kStruct_NAME:
                    current_struct_typename.clear();
                    ConvertOpsToString(ops, current_struct_typename);
                    continue;
                case TypeCodes::kStruct_NAMED:
                case TypeCodes::kStruct_ANON: {
                    if (ops.size() < 1)
                        throw ParserException(ParserError::kDataError);

                    bool ispacked = (ops[0] != 0);
                    std::vector<uint32_t> members;
                    for (size_t i = 1; i < ops.size(); i++) {
                        uint32_t type_index = static_cast<uint32_t>(ops[i]);
                        if (!module_.IsValidTypeIndex(type_index) ||
                            !StructType::IsValidMemberType(module_.type_table[type_index]->GetTypeCode())) {
                            throw ParserException(ParserError::kDataError);
                        }
                        members.push_back(type_index);
                    }

                    if (type_code == TypeCodes::kStruct_NAMED) {
                        result_type = new StructType(ispacked, current_struct_typename);
                        current_struct_typename.clear();
                    } else {
                        result_type = new StructType(ispacked);
                    }
                    if (ops.size() >= 2)
                        static_cast<StructType*>(result_type.Get())->FillMembers(std::move(members));
                    break;
                }
                case TypeCodes::kFunction_Old: {
                    // format: [FUNCTION, vararg, ignored, retty, ...paramty...]
                    if (ops.size() < 3)
                        throw ParserException(ParserError::kDataNotEnough);

                    std::vector<uint32_t> params;
                    for (size_t i = 3; i < ops.size(); i++) {
                        uint32_t param_type = static_cast<uint32_t>(ops[i]);
                        if (!module_.IsValidTypeIndex(param_type))
                            throw ParserException(ParserError::kDataError);
                        if (!FunctionType::IsValidArgumentType(module_.type_table[param_type]->GetTypeCode()))
                            throw ParserException(ParserError::kDataError);
                        params.push_back(param_type);
                    }
                    bool is_vararg = (ops[0] != 0);
                    uint32_t retty = static_cast<uint32_t>(ops[2]);
                    result_type = new FunctionType(is_vararg, retty, std::move(params));
                    break;
                }
                case TypeCodes::kFunction: {
                    // format: [FUNCTION, vararg, retty, ...paramty...]
                    if (ops.size() < 2)
                        throw ParserException(ParserError::kDataNotEnough);

                    std::vector<uint32_t> params;
                    for (size_t i = 2; i < ops.size(); i++) {
                        uint32_t param_type = static_cast<uint32_t>(ops[i]);
                        if (!module_.IsValidTypeIndex(param_type))
                            throw ParserException(ParserError::kDataError);
                        if (!FunctionType::IsValidArgumentType(module_.type_table[param_type]->GetTypeCode()))
                            throw ParserException(ParserError::kDataError);
                        params.push_back(param_type);
                    }
                    bool is_vararg = (ops[0] != 0);
                    uint32_t retty = static_cast<uint32_t>(ops[1]);
                    result_type = new FunctionType(is_vararg, retty, std::move(params));
                    break;
                }
                default:
                    throw ParserException(ParserError::kNotSupproted);
            }
            if (result_type == nullptr)
                throw ParserException(ParserError::kInternalError);
            module_.type_table.push_back(std::move(result_type));
        }
    }

}
}