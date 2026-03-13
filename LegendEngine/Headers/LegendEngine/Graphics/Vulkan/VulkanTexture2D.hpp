#pragma once

#include <LegendEngine/Resources/Texture2D.hpp>

#include <vk_mem_alloc.h>
#include <Tether/Rendering/Vulkan/DescriptorSetWritable.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <vulkan/vulkan.h>

namespace le
{
    class VulkanTexture2D final : public Texture2D,
        public Tether::Rendering::Vulkan::DescriptorSetWritable
    {
    public:
        VulkanTexture2D(Tether::Rendering::Vulkan::GraphicsContext& context,
            const TextureData& loader);
        ~VulkanTexture2D() override;

        VkDescriptorType GetDescriptorType() override;
        VkDescriptorImageInfo GetImageInfo(uint32_t setIndex) override;
    private:
        VkFormat FindImageFormat(uint32_t channels) const;
        void CreateImageView(VkFormat format);
        void CreateSampler();

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;

        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VkSampler m_Sampler = nullptr;
        VmaAllocation m_ImageAllocation = nullptr;
    };
}
