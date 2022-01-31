/*
 * This header contains the definition of a class to be passed to Vulkan
 * for a debug callback.
 */

#ifndef _LEGENDENGINE_VULKAN_DEBUGCALLBACK_HPP
#define _LEGENDENGINE_VULKAN_DEBUGCALLBACK_HPP
#ifdef VULKAN_API

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
    class DebugCallback
    {
    public:
        virtual void OnDebugLog(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        )
        {}
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_DEBUGCALLBACK_HPP
