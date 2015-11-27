#ifndef _BLVM_CORE_TYPE_HPP
#define _BLVM_CORE_TYPE_HPP

#include "../base/ref_base.hpp"
#include "../base/ref_ptr.hpp"

namespace blvm {
namespace core {

    class Type;
    typedef base::RefPtr<Type> TypeRef;

    class Type : public base::RefBase {

    };

}
}

#endif // _BLVM_CORE_TYPE_HPP
