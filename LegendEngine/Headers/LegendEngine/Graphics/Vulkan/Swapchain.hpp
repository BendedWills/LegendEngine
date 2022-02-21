/*
 * The swap chain is a queue of images to present to the screen.
 */

#ifndef _LEGENDENGINE_VULKAN_SWAPCHAIN_HPP
#define _LEGENDENGINE_VULKAN_SWAPCHAIN_HPP
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Ref.hpp>
#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>
#include <LegendEngine/Graphics/Vulkan/Common/SwapchainDetails.hpp>

#include <vulkan/vulkan.hpp>

namespace LegendEngine::Vulkan
{
    class Swapchain : public IDisposable
    {
    public:
        Swapchain() {}

		Swapchain(const Swapchain&) = delete;
		Swapchain(Swapchain&&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain& operator=(Swapchain&&) = delete;

        /**
         * @brief Initializes the swapchain.
         * 
         * @param pSurface A pointer to the surface object to tie the
         *  swapchain to.
         * @param pDevice A pointer to the device to the the swapchain to.
         * @param createInfo A pointer to a VkSwapchainCreateInfoKHR struct
         *  describing how to set up the swapchain.
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(
            Surface* pSurface,
            Device* pDevice,
            SwapchainDetails details,
            VkSwapchainCreateInfoKHR* createInfo
        );
        
        VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR>& presentModes,
            bool vsync);
        VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR& capabilities,
            uint64_t width, uint64_t height);

        uint32_t GetImageCount();
        std::vector<VkImage> GetImages();
        bool GetImageViews(std::vector<VkImageView>* pVec);

        VkExtent2D GetExtent();
        VkFormat GetImageFormat();
        
        VkSwapchainKHR Get();
    protected:
        void OnDispose();
    private:
        uint32_t imageCount = 0;
        VkSwapchainKHR swapchain;

        VkExtent2D extent;
        VkFormat imageFormat;

        Device* pDevice = nullptr;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SWAPCHAIN_HPP