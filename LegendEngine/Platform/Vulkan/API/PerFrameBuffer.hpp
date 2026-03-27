#pragma once

#include <vector>
#include <vk_mem_alloc.h>

#include "GraphicsContext.hpp"
#include "API/Buffer.hpp"

namespace le::vk
{
    class PerFrameBuffer final : public Buffer
    {
    public:
        PerFrameBuffer(GraphicsContext& context, Usage usage, size_t size);
        ~PerFrameBuffer() override;

        void Update(size_t size, const void* data) override;

        void UpdateForAllFrames(size_t size, void* data) override;

        VkBuffer GetBuffer() override;
    private:
        void CreateBuffer(VkBuffer* pBuffer, VmaAllocation* pAlloc, VmaAllocationInfo* pAllocInfo,
            VkBufferUsageFlags bufferUsage, VmaAllocationCreateFlags allocCreateFlags, size_t size) const;

        TetherVulkan::GraphicsContext& m_context;

        std::vector<VkBuffer> m_buffers;
        std::vector<VmaAllocation> m_allocations;
        std::vector<VmaAllocationInfo> m_allocationInfos;
    };
}
