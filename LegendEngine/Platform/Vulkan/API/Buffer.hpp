#pragma once

#include <LE/Graphics/API/Buffer.hpp>

#include <vk_mem_alloc.h>

namespace le::vk
{
    class Buffer : public le::Buffer
    {
    public:
        virtual VkBuffer GetBuffer() = 0;
    protected:
        static VkBufferUsageFlags ToVulkanUsageFlags(Usage usage);
        static VmaAllocationCreateFlags ToVmaAllocationCreateFlags(Usage usage, bool mapped);
    };
}
