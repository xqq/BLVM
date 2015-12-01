#include "logging.hpp"

#ifdef _WIN32

#include <cstdlib>
#include <cstdio>
#include <Windows.h>

namespace blvm {
namespace base {

    void Win32Logger::PrintLog(int level, const char *format, va_list args) {
        if (IsDebuggerPresent()) {
            char buffer[4096];
            vsnprintf(buffer, sizeof(buffer), format, args);
            OutputDebugStringA(buffer);
            OutputDebugStringA("\n");
        } else {
            fprintf(stderr, format, args);
            fprintf(stderr, "\n");
        }
    }

}
}

#endif