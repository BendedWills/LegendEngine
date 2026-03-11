#pragma once

#ifdef _WIN32
#ifdef _WIN64
#define LGENG_PLATFORM_WINDOWS
#else
#error "x86 is not supported"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#define LGENG_PLATFORM_MACOS
#elif defined(__linux__)
#define LGENG_PLATFORM_LINUX
#else
#error "Unsupported platform"
#endif


