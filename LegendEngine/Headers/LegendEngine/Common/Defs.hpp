#pragma once

#include <sstream>
#include <cassert>

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
#define LEGENDENGINE_NO_COPY(typename) \
    typename(const typename&) = delete; \
    typename(typename&&) = delete; \
    typename& operator=(const typename&) = delete; \
    typename& operator=(typename&&) = delete;
