#ifndef _BLVM_BITCODE_PARSING_CONTEXT_HPP
#define _BLVM_BITCODE_PARSING_CONTEXT_HPP

#include <vector>
#include "../base/memory_buffer.hpp"
#include "bitcode_base.hpp"

namespace blvm {
namespace bitcode {

    class ParsingContext {
    public:

    private:
        base::MemoryBuffer bitcode_storage_;
        std::vector<BlockInfo> block_infos;
    };

}
}

#endif // _BLVM_BITCODE_PARSING_CONTEXT_HPP
