#include "API/SimpleBuffer.hpp"

#include <VkDefs.hpp>
#include <LE/IO/Logger.hpp>

namespace le::vk
{
    SimpleBuffer::SimpleBuffer(GraphicsContext& context, const Usage usage, const size_t size, const bool createMapped)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = ToVulkanUsageFlags(usage);

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = ToVmaAllocationCreateFlags(usage, createMapped);

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

    void SimpleBuffer::Resize(size_t newSize)
    {
        // TODO
    }

    size_t SimpleBuffer::GetSize()
    {
        return m_allocationInfo.size;
    }

    Buffer::Desc SimpleBuffer::GetDesc()
    {
        return {
            m_buffer,
            m_allocationInfo.size,
        };
    }

    void* SimpleBuffer::GetMappedData()
    {
        return m_allocationInfo.pMappedData;
    }
}
