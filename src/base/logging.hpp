#ifndef _BLVM_CORE_LOGGING_HPP
#define _BLVM_CORE_LOGGING_HPP

#include <cstdarg>
#include <cstdio>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#define BLVM_LOG_TAG "BLVM"

#define BLVM_LOG_LEVEL_DEBUG 3
#define BLVM_LOG_LEVEL_INFO  4
#define BLVM_LOG_LEVEL_WARN  5
#define BLVM_LOG_LEVEL_ERROR 6


#if defined(__ANDROID__)
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, BLVM_LOG_TAG, __VA_ARGS__))
    #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, BLVM_LOG_TAG, __VA_ARGS__))
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, BLVM_LOG_TAG, __VA_ARGS__))

    #ifndef NDEBUG
        #define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, BLVM_LOG_TAG, __VA_ARGS__))
    #else
        #define LOGD(...) ((void)0)
    #endif
#elif defined(__APPLE__)
    #define LOGI(...) ((void)fprintf(stderr, __VA_ARGS__))
    #define LOGE(...) LOGI(__VA_ARGS__)
    #define LOGW(...) LOGI(__VA_ARGS__)

    #ifndef NDEBUG
        #define LOGD(...) LOGI(__VA_ARGS__)
    #else
        #define LOGD(...) ((void)0)
    #endif
#elif defined(_WIN32)

#else

#endif


namespace blvm {
namespace base {

#ifdef _WIN32
    class Win32Logger {
    public:
        static void PrintLog(int level, const char* format, va_list args);
    };
#endif

}
}

#endif // _BLVM_CORE_LOGGING_HPP
