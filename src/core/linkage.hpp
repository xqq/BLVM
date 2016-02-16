#ifndef _BLVM_CORE_LINKAGE_HPP
#define _BLVM_CORE_LINKAGE_HPP

namespace blvm {
namespace core {

    enum Linkage {
        kExternal = 0,
        kWeak = 1,
        kAppending = 2,
        kInternal = 3,
        kLinkonce = 4,
        kDllImport = 5,
        kDllExport = 6,
        kExtern_Weak = 7,
        kCommon = 8,
        kPrivate = 9,
        kWeak_Odr = 10,
        kLinkonce_Odr = 11,
        kAvailableExternally = 12,
        kDeprecated = 13,
        kDeprecated2 = 14
    };

}
}

#endif // _BLVM_CORE_LINKAGE_HPP
