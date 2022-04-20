#ifndef _LEGENDENGINE_VULKAN_TEXTURE2DNATIVE_HPP
#define _LEGENDENGINE_VULKAN_TEXTURE2DNATIVE_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class Texture2DNative : public Resources::ITexture2DNative
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_NO_COPY(Texture2DNative);
        
        Texture2DNative(VulkanRenderer* pRenderer, Resources::Texture2D* pTexture)
            :
            pVulkanRenderer(pRenderer),
            Resources::ITexture2DNative(pTexture)
        {}
    protected:
        VkImage image;
        VmaAllocation imageAllocation;

        VkImageView imageView;

        VkBuffer stagingBuffer;
        VmaAllocation stagingAlloc;
        VmaAllocationInfo stagingAllocInfo;

        uint64_t width;
        uint64_t height;
        uint32_t channels;

        VkFormat imageFormat;
        
        VulkanRenderer* pVulkanRenderer = nullptr;
    private:
		bool OnCreate(uint64_t width, uint64_t height, uint32_t channels, 
            uint8_t* data);
		void OnDispose();

        bool StageImageData(uint8_t* data);
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_TEXTURE2DNATIVE_HPP