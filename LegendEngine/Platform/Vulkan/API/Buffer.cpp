#include "API/Buffer.hpp"

namespace le::vk
{
    VkBufferUsageFlags Buffer::ToVulkanUsageFlags(const Usage usage)
    {
        VkBufferUsageFlags vkUsage = 0;
        switch (usage)
        {
            case Usage::TRANSFER_SRC: vkUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; break;
            case Usage::UNIFORM_BUFFER: vkUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
            case Usage::VERTEX_BUFFER: vkUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
            case Usage::INDEX_BUFFER: vkUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
        }

        return vkUsage;
    }

    VmaAllocationCreateFlags Buffer::ToVmaAllocationCreateFlags(const Usage usage, bool mapped)
    {
        VmaAllocationCreateFlags flags;
        switch (usage)
        {
            case Usage::TRANSFER_SRC:
            case Usage::UNIFORM_BUFFER:
            {
                flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            }
            break;

            default: flags = 0;
        }

        if (mapped)
            flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

        return flags;
    }
}
