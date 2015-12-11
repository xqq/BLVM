#ifndef _BLVM_BITCODE_BITCODE_LLVM_HPP
#define _BLVM_BITCODE_BITCODE_LLVM_HPP

#include "bitcode_base.hpp"

namespace blvm {
namespace bitcode {

    enum BlockIds {
        kModuleBlock = StandardBlockIds::kFirstApplicationBlockId,

        kParamattrBlock,
        kParamattrGroupBlock,

        kConstantBlock,
        kFunctionBlock,

        kUnusedId1,

        kValueSymtabBlock,
        kMetadataBlock,
        kMetadataAttachment,

        kTypeBlock,

        kUselistBlock
    };

    enum ModuleCodes {
        kVersion = 1,
        kTriple = 2,
        kDataLayout = 3,
        kAsm = 4,
        kSectionName = 5,

        // intended to remove by llvm
        kDepLib = 6,

        kGlobalVar = 7,
        kFunction = 8,
        kAlias = 9,
        kPurgeVals = 10,

        kGcName = 11,
        kComdat = 12
    };

    enum AttributeCodes {
        kEntryOld = 1,
        kEntry = 2,
        kGrpCodeEntry = 3
    };

    enum class TypeCodes : uint32_t {
        kNumEntry = 1,

        kVoid = 2,
        kFloat = 3,
        kDouble = 4,
        kLabel = 5,
        kOpaque = 6,
        kInteger = 7,
        kPointer = 8,

        kFunction_Old = 9,

        kHalf = 10,

        kArray = 11,
        kVector = 12,

        kX86_FP80 = 13,
        kFP128 = 14,
        kPPC_FP128 = 15,

        kMetadata = 16,

        kX86_MMX = 17,

        kStruct_ANON = 18,
        kStruct_NAME = 19,
        kStruct_NAMED = 20,

        kFunction = 21
    };

}
}

#endif // _BLVM_BITCODE_BITCODE_LLVM_HPP
