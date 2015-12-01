#include "bitcode_reader.hpp"

namespace blvm {
namespace bitcode {

    BitcodeReader::BitcodeReader(core::BLVMContext& context, core::Module& module, base::MemoryBuffer& bitcode_buffer) :
            context_(context), module_(module), buffer_(bitcode_buffer),
            buffer_index_(0), current_word_(0), current_word_bits_left_(0), current_block_(2) {

    }

    BitcodeReader::~BitcodeReader() {

    }

    void BitcodeReader::FillCurrentWord() {

    }

}
}