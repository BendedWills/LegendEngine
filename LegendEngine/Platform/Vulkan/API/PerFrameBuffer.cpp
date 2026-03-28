#include "API/PerFrameBuffer.hpp"

#include <VkDefs.hpp>
#include <LE/Application.hpp>

namespace le::vk
{
    PerFrameBuffer::PerFrameBuffer(GraphicsContext& context, Usage usage, size_t size)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        m_buffers.resize(Application::FRAMES_IN_FLIGHT);
        m_allocations.resize(Application::FRAMES_IN_FLIGHT);
        m_allocationInfos.resize(Application::FRAMES_IN_FLIGHT);

        const VkBufferUsageFlags bufferUsage = ToVulkanUsageFlags(usage);
        const VmaAllocationCreateFlags allocCreateFlags = ToVmaAllocationCreateFlags(usage, true);
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; i++)
            CreateBuffer(&m_buffers[i], &m_allocations[i], &m_allocationInfos[i], bufferUsage, allocCreateFlags, size);
    }

    PerFrameBuffer::~PerFrameBuffer()
    {
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; i++)
            vmaDestroyBuffer(m_context.GetAllocator(), m_buffers[i], m_allocations[i]);
    }

    void PerFrameBuffer::Update(const size_t size, const void* data)
    {
        const size_t currentFrame = Application::Get().GetCurrentFrame();
        const size_t allocSize = m_allocationInfos[currentFrame].size;
        void* pMappedData = m_allocationInfos[currentFrame].pMappedData;

        LE_ASSERT(pMappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size <= allocSize, "Tried to copy too much data into buffer");

        memcpy(pMappedData, data, size);
    }

    void PerFrameBuffer::Resize(size_t newSize)
    {
        // TODO
    }

    size_t PerFrameBuffer::GetSize()
    {
        return m_allocationInfos[Application::Get().GetCurrentFrame()].size;
    }

    Buffer::Desc PerFrameBuffer::GetDesc()
    {
        return {
            m_buffers[Application::Get().GetCurrentFrame()],
            m_allocationInfos[Application::Get().GetCurrentFrame()].size,
        };
    }

    void PerFrameBuffer::CreateBuffer(VkBuffer* pBuffer, VmaAllocation* pAlloc, VmaAllocationInfo* pAllocInfo,
                                      const VkBufferUsageFlags bufferUsage, const VmaAllocationCreateFlags allocCreateFlags, const size_t size) const
    {

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = bufferUsage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = allocCreateFlags;

        LE_CHECK_VK(vmaCreateBuffer(m_context.GetAllocator(), &bufferCreateInfo, &allocInfo,
            pBuffer, pAlloc, pAllocInfo));
    }
}
