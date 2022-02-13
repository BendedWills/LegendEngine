#ifndef _LEGENDENGINE_VULKANRENDERER_HPP
#define _LEGENDENGINE_VULKANRENDERER_HPP
#ifdef VULKAN_API

#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>
#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>
#include <LegendEngine/Graphics/Vulkan/Swapchain.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>

#include <Tether/Tether.hpp>

#include <iostream>
#include <vector>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer : public IRenderer
    {
    public:
        VulkanRenderer() 
            :
            eventHandler(this)
        {}
        
        VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        void SetVSyncEnabled(bool vsync);

        /**
         * @brief Reloads the renderer. Required after a settings change.
         * 
         * @returns True if the reload was successful; otherwise, false.
         */
        bool Reload();
    private:
        bool RecreateSwapchain(uint64_t width, uint64_t height);

        bool OnRendererInit();
        bool OnRenderFrame();
        void OnRendererDispose();

        bool PickDevice(VkPhysicalDevice* pDevice, Vulkan::Surface* pSurface);
        bool IsDeviceSuitable(VkPhysicalDevice device, 
            Vulkan::Surface* pSurface);
        bool InitDevice();
        VkSurfaceFormatKHR ChooseSurfaceFormat(Vulkan::SwapchainDetails details);
        bool InitSwapchain(uint64_t width, uint64_t height);
        bool InitShaders();
        bool InitRenderPass();
        bool InitPipeline();
        bool InitFramebuffers();
        bool InitCommandPool();
        bool InitCommandBuffers();
        bool InitSyncObjects();

        bool DrawFrame();

        void DisposeSwapchain();

        Vulkan::Surface surface;
        Vulkan::Device device;
        Vulkan::Swapchain swapchain;
        
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        // Vulkan::Buffer vertexBuffer; // Unimplemented
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;
        VkPipeline pipeline;
        VkCommandPool commandPool;
        // Shader stuff
        Vulkan::ShaderModule vertexModule;
        Vulkan::ShaderModule fragmentModule;

        Vulkan::QueueFamilyIndices indices;
        VkPhysicalDevice physicalDevice;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkCommandBuffer> commandBuffers;

        const int MAX_FRAMES_IN_FLIGHT = 2;
        uint64_t currentFrame = 0;

        bool enableVsync = false;

        class EventHandler : public Tether::Events::EventHandler
        {
        public:
            EventHandler(VulkanRenderer* pRenderer);
            void OnWindowResize(Tether::Events::WindowResizeEvent event);
        private:
            VulkanRenderer* pRenderer;
        };
        EventHandler eventHandler;

        const std::vector<const char*> deviceExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        
        IApplication* pApplication = nullptr;
        Vulkan::Instance* pInstance;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKANRENDERER_HPP