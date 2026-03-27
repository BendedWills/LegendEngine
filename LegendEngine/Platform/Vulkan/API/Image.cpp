#include "API/Image.hpp"

#include <VkDefs.hpp>
#include <LE/Common/Assert.hpp>

namespace le::vk
{
    Image::Image(GraphicsContext& context, const Info& info)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        VkFormat format = VK_FORMAT_R8G8B8_SRGB;
        switch (info.format)
        {
            case Format::R8: format = VK_FORMAT_R8_SRGB; break;
            case Format::R8G8: format = VK_FORMAT_R8G8_SRGB; break;
            case Format::R8G8B8: format = VK_FORMAT_R8G8B8_SRGB; break;
            case Format::R8G8B8A8: format = VK_FORMAT_R8G8B8A8_SRGB; break;

            default: LE_ASSERT(false, "Unknown image format"); break;
        }

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = info.width;
        imageInfo.extent.height = info.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = info.arrayLayers;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.format = format;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        LE_CHECK_VK(vmaCreateImage(
            m_context.GetAllocator(),
            &imageInfo,
            &allocInfo,
            &m_image,
            &m_allocation,
            nullptr
        ));

        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
        switch (info.type)
        {
            case Type::TYPE_2D: viewType = VK_IMAGE_VIEW_TYPE_2D; break;
            case Type::TYPE_2D_ARRAY: viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY; break;

            default: LE_ASSERT(false, "Unknown image type"); break;
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.format = format;
        viewInfo.viewType = viewType;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = info.arrayLayers;

        LE_CHECK_VK(vkCreateImageView(m_context.GetDevice(), &viewInfo, nullptr, &m_view));
    }

    Image::~Image()
    {
        vmaDestroyImage(m_context.GetAllocator(), m_image, m_allocation);
        vkDestroyImageView(m_context.GetDevice(), m_view, nullptr);
    }
}
