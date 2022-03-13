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
        /**
         * @brief Called when any message is recieved from vulkan validation
         *  layers.
         * 
         * @param messageSeverity The severity of the message (wow really)
         * @param messageType uhh
         * @param pCallbackData The callback data... I guess.
         */
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
