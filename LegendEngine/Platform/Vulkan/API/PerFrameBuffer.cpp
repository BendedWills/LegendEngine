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

    void PerFrameBuffer::UpdateForAllFrames(const size_t size, void* data)
    {
        LE_ASSERT(m_allocationInfos[0].pMappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size <= m_allocationInfos[0].size, "Tried to copy too much data into buffer");

        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; i++)
            memcpy(m_allocationInfos[i].pMappedData, data, size);
    }

    VkBuffer PerFrameBuffer::GetBuffer()
    {
        return m_buffers[Application::Get().GetCurrentFrame()];
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
