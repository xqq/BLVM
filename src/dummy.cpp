#include <cstdio>
#include "base/ref_base.hpp"
#include "base/ref_ptr.hpp"
#include "base/memory_buffer.hpp"
#include "base/error_handling.hpp"
#include "bitcode/bitcode_base.hpp"
#include "bitcode/bitcode_llvm.hpp"
#include "bitcode/bitcode_reader.hpp"
#include "bitcode/bitcode_parser.hpp"
#include "bitcode/parsing_exception.hpp"
#include "core/blvm_context.hpp"
#include "core/module.hpp"
#include "base/logging.hpp"

namespace blvm {

    class FooClass : public base::RefBase {
    public:
        FooClass() = default;

        virtual ~FooClass() {
            printf("destructor for FooClass\n");
        }

        int Fuck(int i) {
            return dummy_integer_ + i;
        }

    private:
        int dummy_integer_ = 12450;
    };

    int dummy_parse(const char* filename) {
        base::RefPtr<FooClass> pfoo = new FooClass;

        FILE* input_file = fopen(filename, "rb");
        DCHECK(input_file);

        fseek(input_file, 0, SEEK_END);
        size_t size = static_cast<size_t>(ftell(input_file));
        fseek(input_file, 0, SEEK_SET);

        base::MemoryBuffer buffer(base::MemoryBuffer::kAllocateInternally, size);
        fread(const_cast<uint8_t*>(buffer.begin()), 1, size, input_file);

        fclose(input_file);

        core::BLVMContext context;
        bitcode::ParsingContext parsing_context(std::move(buffer));
        core::Module module;

        bitcode::BitcodeReader reader(parsing_context, *parsing_context.GetBitcodeBuffer());
        bitcode::BitcodeParser parser(context, parsing_context, reader, module);

        parser.Parse();

        printf("module_version: %d\n", module.module_version);
        printf("datalayout: %s\n", module.target_datalayout.c_str());
        printf("target_triple: %s\n\n", module.target_triple.c_str());


        return pfoo->Fuck(233);
    }

}