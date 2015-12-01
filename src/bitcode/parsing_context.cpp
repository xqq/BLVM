#include "parsing_context.hpp"
#include <utility>
#include "bitcode_base.hpp"

namespace blvm {
namespace bitcode {

    ParsingContext::ParsingContext(base::MemoryBuffer&& bitcode_buffer) :
            bitcode_storage_(std::move(bitcode_buffer)) {}

    bool ParsingContext::HasBlockInfos() {
        return !block_infos.empty();
    }

    BlockInfo* ParsingContext::GetBlockInfo(int block_id) {
        if (!block_infos.empty() && block_infos.back().block_id == block_id)
            return &block_infos.back();

        int size = (int)block_infos.size();
        for (int i = 0; i < size; i++) {
            if (block_infos[i].block_id == block_id)
                return &block_infos[i];
        }
        return nullptr;
    }

    BlockInfo* ParsingContext::GetOrCreateBlockInfo(int block_id) {
        BlockInfo* info = GetBlockInfo(block_id);
        if (info)
            return info;

        block_infos.push_back(BlockInfo());
        block_infos.back().block_id = block_id;
        return &block_infos.back();
    }

}
}