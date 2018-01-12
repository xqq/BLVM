#include <cstdio>

namespace blvm {
    extern int dummy_parse(const char* filename);
}

int main(int argc, char** argv) {
    int ret = blvm::dummy_parse("D:\\em-workspace\\test.bc");
    printf("Hello world! ret = %d\n", ret);
    return 0;
}