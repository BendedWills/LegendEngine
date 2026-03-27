#pragma once

#include <vk_mem_alloc.h>

#include "GraphicsContext.hpp"
#include "API/Buffer.hpp"

namespace le::vk
{
    class SimpleBuffer final : public Buffer
    {
    public:
        SimpleBuffer(GraphicsContext& context, Usage usage, size_t size, bool createMapped);
        ~SimpleBuffer() override;

        void Update(size_t size, const void* data) override;
        void Resize(size_t newSize) override;

        void* GetMappedData() override;

        VkBuffer GetBuffer() override;
    private:
        TetherVulkan::GraphicsContext& m_context;

        VkBuffer m_buffer = nullptr;
        VmaAllocation m_allocation = nullptr;
        VmaAllocationInfo m_allocationInfo{};
    };
}
