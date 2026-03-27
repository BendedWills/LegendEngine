#include "API/SimpleBuffer.hpp"

#include <VkDefs.hpp>
#include <LE/IO/Logger.hpp>

namespace le::vk
{
    SimpleBuffer::SimpleBuffer(GraphicsContext& context, const Usage usage, const size_t size, const bool createMapped)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        VkBufferUsageFlags vkUsage = 0;
        VmaAllocationCreateFlags flags = 0;
        switch (usage)
        {
            case Usage::TRANSFER_SRC:
            {
                vkUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            }
            break;

            case Usage::UNIFORM_BUFFER:
            {
                vkUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            }
            break;

            case Usage::VERTEX_BUFFER: vkUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
            case Usage::INDEX_BUFFER: vkUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
        }

        if (createMapped)
            flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = vkUsage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = flags;

        LE_CHECK_VK(vmaCreateBuffer(m_context.GetAllocator(), &bufferCreateInfo, &allocInfo,
            &m_buffer, &m_allocation, &m_allocationInfo));
    }

    SimpleBuffer::~SimpleBuffer()
    {
        vmaDestroyBuffer(m_context.GetAllocator(), m_buffer, m_allocation);
    }

    void SimpleBuffer::Update(const size_t size, const void* data)
    {
        LE_ASSERT(m_allocationInfo.pMappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size <= m_allocationInfo.size, "Tried to copy too much data into buffer");
        memcpy(m_allocationInfo.pMappedData, data, size);
    }

    void SimpleBuffer::UpdateForAllFrames(const size_t size, void* data)
    {
        Update(size, data);
    }

    VkBuffer SimpleBuffer::GetBuffer()
    {
        return m_buffer;
    }

    void* SimpleBuffer::GetMappedData()
    {
        return m_allocationInfo.pMappedData;
    }
}
