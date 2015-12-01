#ifndef _BLVM_BITCODE_PARSING_CONTEXT_HPP
#define _BLVM_BITCODE_PARSING_CONTEXT_HPP

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
        BlockInfo* GetBlockInfo(int block_id);
        BlockInfo* GetOrCreateBlockInfo(int block_id);
    private:
        base::MemoryBuffer bitcode_storage_;
        std::vector<BlockInfo> block_infos;

        DISALLOW_COPY_AND_ASSIGN(ParsingContext);
    };

}
}

#endif // _BLVM_BITCODE_PARSING_CONTEXT_HPP
