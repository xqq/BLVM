#include <cstdio>

extern int dummy_func(void* ptr);

int main(int argc, char** argv) {
    int i = 233;
    int ret = dummy_func(&i);
    printf("Hello world! ret = %d\n", ret);
    return 0;
}