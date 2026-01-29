#include <LegendEngine/Graphics/Vulkan/VulkanTexture2D.hpp>
#include <Tether/Rendering/Vulkan/ImageStager.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Tether::Rendering::Vulkan;

    VulkanTexture2D::VulkanTexture2D(GraphicsContext& context,
        const IO::TextureLoader& loader)
        :
        Texture2D(loader),
        m_Context(context)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = loader.GetWidth();
        imageInfo.extent.height = loader.GetHeight();
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.format = FindImageFormat(loader.GetChannels());

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (vmaCreateImage(
                m_Context.GetAllocator(),
                &imageInfo,
                &allocInfo,
                &m_Image,
                &m_ImageAllocation,
                nullptr
            ) != VK_SUCCESS)
            throw std::runtime_error("Failed to create Texture2D");

        CreateImageView(imageInfo.format);
        CreateSampler();

        ImageStager stager(
            m_Context, m_Image,
            GetWidth(), GetHeight(), GetChannels(),
            loader.GetData(),
            imageInfo.format
        );
        stager.Upload();
    }

    VulkanTexture2D::~VulkanTexture2D()
    {
        const VkDevice device = m_Context.GetDevice();

        vkDeviceWaitIdle(device);

        vkDestroyImageView(device, m_ImageView, nullptr);
        vkDestroySampler(device, m_Sampler, nullptr);
        vmaDestroyImage(m_Context.GetAllocator(), m_Image, m_ImageAllocation);
    }

    VkDescriptorType VulkanTexture2D::GetDescriptorType()
    {
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }

    VkDescriptorImageInfo VulkanTexture2D::GetImageInfo(uint32_t setIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_ImageView;
        imageInfo.sampler = m_Sampler;

        return imageInfo;
    }

    VkFormat VulkanTexture2D::FindImageFormat(uint32_t channels) const
    {
        VkFormat format;
        switch (channels)
        {
            case 1: format = VK_FORMAT_R8_SRGB; break;
            case 3: format = VK_FORMAT_R8G8B8_SRGB; break;
            case 4: format = VK_FORMAT_R8G8B8A8_SRGB; break;

            default: throw std::runtime_error("Unsupported image format");
        }

        return format;
    }

    void VulkanTexture2D::CreateImageView(const VkFormat format)
    {
        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = m_Image;
        viewCreateInfo.format = format;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_Context.GetDevice(), &viewCreateInfo, nullptr, &m_ImageView)
            != VK_SUCCESS)
        {
            vkDeviceWaitIdle(m_Context.GetDevice());
            vmaDestroyImage(m_Context.GetAllocator(), m_Image, m_ImageAllocation);
            throw std::runtime_error("Failed to create image view");
        }
    }

    void VulkanTexture2D::CreateSampler()
    {
        VkSamplerCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.minFilter = VK_FILTER_LINEAR;
        createInfo.magFilter = VK_FILTER_LINEAR;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.anisotropyEnable = false;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_Context.GetPhysicalDevice(), &properties);

        createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        createInfo.unnormalizedCoordinates = false;
        createInfo.compareEnable = false;
        createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.mipLodBias = 0.0f;
        createInfo.minLod = 0.0f;
        createInfo.maxLod = 0.0f;

        if (vkCreateSampler(m_Context.GetDevice(), &createInfo, nullptr, &m_Sampler)
            != VK_SUCCESS)
        {
            vkDeviceWaitIdle(m_Context.GetDevice());
            vmaDestroyImage(m_Context.GetAllocator(), m_Image, m_ImageAllocation);
            vkDestroyImageView(m_Context.GetDevice(), m_ImageView, nullptr);
            throw std::runtime_error("Failed to create texture sampler");
        }
    }
}
