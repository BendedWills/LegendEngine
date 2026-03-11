#pragma once

#include <LegendEngine/Common/Platform.hpp>
#include <LegendEngine/IO/Logger.hpp>

#ifndef NDEBUG
#ifdef LGENG_PLATFORM_WINDOWS
#define LGENG_DEBUGBREAK() __debugbreak()
#elif defined(LGENG_PLATFORM_MACOS) || defined(LGENG_PLATFORM_LINUX)
#include <signal.h>
#define LGENG_DEBUGBREAK() raise(SIGTRAP)
#endif

#define LGENG_ASSERT(check, ...) do { if (!(check)) { LGENG_ERROR_TRACE(__VA_ARGS__); LGENG_DEBUGBREAK(); } } while(0)
#else
#define LGENG_DEBUGBREAK()
#define LGENG_ASSERT(...)
#endif