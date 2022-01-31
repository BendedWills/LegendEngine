#ifndef _LEGENDENGINE_VULKANRENDERER_HPP
#define _LEGENDENGINE_VULKANRENDERER_HPP
#ifdef VULKAN_API

#include <LegendEngine/IRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>
#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>
#include <LegendEngine/Graphics/Vulkan/Swapchain.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>

#include <iostream>
#include <vector>

namespace LegendEngine
{
    class VulkanRenderer : public IRenderer
    {
    public:
        VulkanRenderer() {}
        
        VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        void SetValidationLayersEnabled(bool enableValidationLayers);
    private:
        class DebugCallback : public Vulkan::DebugCallback
        {
        public:
            Application* pApplication = nullptr;

            void OnDebugLog(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
            );
        };

        bool OnRendererInit();
        void OnRendererDispose();

        bool PickDevice(VkPhysicalDevice* pDevice, Vulkan::Surface* pSurface);
        bool IsDeviceSuitable(VkPhysicalDevice device, 
            Vulkan::Surface* pSurface);
        bool InitDevice();
        VkSurfaceFormatKHR ChooseSurfaceFormat(Vulkan::SwapchainDetails details);
        bool InitSwapchain(uint64_t width, uint64_t height);

        Vulkan::Instance instance;
        Vulkan::Surface surface;
        Vulkan::Device device;
        Vulkan::Swapchain swapchain;
        
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;

        DebugCallback callback;

        Vulkan::QueueFamilyIndices indices;
        VkPhysicalDevice physicalDevice;

        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        bool enableValidationLayers = false;

        const std::vector<const char*> deviceExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKANRENDERER_HPP