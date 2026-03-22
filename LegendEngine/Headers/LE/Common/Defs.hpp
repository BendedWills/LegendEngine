#pragma once

#include <cassert>
#include <sstream>

#ifdef __INTELLISENSE__
    #ifndef VULKAN_API
    #define VULKAN_API
    #endif // VULKAN_API

    #ifndef OPENGL_API
    #define OPENGL_API
    #endif // OPENGL_API
#endif //__INTELLISENSE__

/**
 * @brief Disables object copying.
 */
#define LE_NO_COPY(typename) \
    typename(const typename&) = delete; \
    typename(typename&&) = delete; \
    typename& operator=(const typename&) = delete; \
    typename& operator=(typename&&) = delete;

#if defined(NDEBUG)
#define LEGENDENGINE_RELEASE
#else
#define LEGENDENGINE_DEBUG
#endif
