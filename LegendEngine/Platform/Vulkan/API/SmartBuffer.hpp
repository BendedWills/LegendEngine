#pragma once

#include <atomic>
#include <BufferStager.hpp>
#include <utility>
#include <LE/Events/EventBusSubscriber.hpp>

#include "GraphicsContext.hpp"
#include "API/Buffer.hpp"

namespace le::vk
{
    class SmartBuffer final : public Buffer
    {
    public:
        SmartBuffer(GraphicsContext& context, Usage usage, size_t initialSize);
        ~SmartBuffer() override;

        void Update(size_t size, const void* data) override;
        void Resize(size_t newSize) override;

        VkBuffer GetBuffer() override;
    private:
        struct BufferDesc
        {
            std::atomic<VkBuffer> buffer = nullptr;
            std::atomic<VmaAllocation> allocation = nullptr;
            std::atomic<size_t> size = 0;
        };

        [[nodiscard]] std::pair<VkBuffer, VmaAllocation> CreateBuffer(VkBufferUsageFlags flags, size_t size) const;
        BufferDesc* AcquireUnusedBuffer();
        void DestroyBuffer(BufferDesc& buffer) const;

        void DeleteUnusedBuffers();
        void DeleteStager();

        TetherVulkan::GraphicsContext& m_context;

        VkBufferUsageFlags m_usage;

        BufferDesc m_buffer1;
        BufferDesc m_buffer2;

        std::mutex m_updateMutex;
        std::atomic<BufferDesc*> m_currentBuffer = nullptr;
        std::atomic<BufferDesc*> m_updatedBuffer = nullptr;
        std::atomic_size_t m_size = 0;

        size_t m_framesSinceDeletion = 0;

        EventBusSubscriber m_sub;

        BufferStager m_stager;
    };
}
