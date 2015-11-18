#include <cstdio>
#include "base/ref_base.hpp"
#include "base/ref_ptr.hpp"

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

int dummy_func(void* ptr) {
    base::RefPtr<FooClass> pfoo = new FooClass;
    return pfoo->Fuck(*reinterpret_cast<int*>(ptr));
}