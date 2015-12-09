#ifndef _BLVM_BITCODE_PARSING_CONTEXT_HPP
#define _BLVM_BITCODE_PARSING_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include <utility>
#include "../base/memory_buffer.hpp"

namespace blvm {
namespace bitcode {

    struct BlockInfo;

    class ParsingContext {
    public:
        explicit ParsingContext(base::MemoryBuffer&& bitcode_buffer);
        bool HasBlockInfos();
        BlockInfo* GetBlockInfo(uint32_t block_id);
        BlockInfo* GetOrCreateBlockInfo(uint32_t block_id);
    private:
        base::MemoryBuffer bitcode_storage_;
        std::vector<BlockInfo> block_infos;

        DISALLOW_COPY_AND_ASSIGN(ParsingContext);
    };

}
}

#endif // _BLVM_BITCODE_PARSING_CONTEXT_HPP
