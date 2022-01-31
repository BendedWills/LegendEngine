/*
 * This header represents the wrapper for a Vulkan device.
 */

#ifndef _LEGENDENGINE_VULKAN_DEVICE_HPP
#define _LEGENDENGINE_VULKAN_DEVICE_HPP
#ifdef VULKAN_API


#include <vector>

#include <LegendEngine/Common/IDisposable.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
    class Instance;
    class Device : public IDisposable
    {
    public:
        Device() {}

		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;
        
        /**
         * @brief Initializes the device.
         * 
         * @param pInstance A pointer to the Vulkan instance.
         * @param physicalDevice The physical device.
         * @param queueFamilies An array of VkDeviceQueueCreateInfo specifying
         *  the queue families that this device should use and describing them.
         * @param queueFamilyCount The number of VkDeviceQueueCreateInfo
         *  in the array.
         * @param features The features that this device should use.
         * @param extentions An array of extentions for this device to use.
         * @param extentionCount The number of extentions.
         * 
         * @returns True if initialization succeeded. Otherwise, false.
         */
        bool Init(
            Instance* pInstance,
            VkPhysicalDevice physicalDevice,
            VkDeviceQueueCreateInfo* queueFamilies, 
            uint64_t queueFamilyCount, 
            VkPhysicalDeviceFeatures features,
            const char *const * extentions = nullptr,
            uint64_t extentionCount = 0
        );

        /**
         * @brief Get the device queue at the specified index.
         * 
         * @param familyIndex The queue family.
         * @param queueIndex The index of the queue in the family.
         * 
         * @returns The queue object.
         */
        VkQueue GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex);

        void WaitIdle();
        
        VkDevice Get();
    protected:
        void OnDispose();
    private:
        VkDevice device;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_DEVICE_HPP
