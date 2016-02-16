#ifndef _BLVM_BASE_EXPORT_HPP
#define _BLVM_BASE_EXPORT_HPP

#define BLVM_IMPL_SIDE 1

#if defined(BLVM_DLL)
    #if defined(_MSC_VER) || defined(_WIN32)
        #if BLVM_IMPL_SIDE
            #define BLVM_API __declspec(dllexport)
        #else
            #define BLVM_API __declspec(dllimport)
        #endif
    #elif defined(__GNUC__)
        #define BLVM_API __attribute__((visibility("default")))
    #else
        #define BLVM_API
    #endif
#else
    #define BLVM_API
#endif


#endif // _BLVM_BASE_EXPORT_HPP
