#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class MaterialNative;
    class Texture2DNative : public Resources::ITexture2DNative
    {
        friend VulkanRenderer;
        friend MaterialNative;
    public:
        LEGENDENGINE_NO_COPY(Texture2DNative);
        
        Texture2DNative(TetherVulkan::GraphicsContext& context,
            Resources::Texture2D* pTexture);
    protected:
        VkSampler sampler;

        VkImage image;
        VmaAllocation imageAllocation;

        VkImageView imageView;

        VkBuffer stagingBuffer;
        VmaAllocation stagingAlloc;
        VmaAllocationInfo stagingAllocInfo;

        std::optional<TetherVulkan::ImageStager> m_Stager;

        uint64_t width;
        uint64_t height;
        uint32_t channels;

        VkFormat imageFormat;
        
        TetherVulkan::GraphicsContext& m_Context;
        VkDevice m_Device = nullptr;
    private:
		bool OnCreate(uint64_t width, uint64_t height, uint32_t channels, 
            uint8_t* data);
		void OnDispose();

        void StageImageData(uint8_t* data);
    };
}
