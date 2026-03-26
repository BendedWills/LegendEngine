#pragma once

#include <vk_mem_alloc.h>
#include <LE/Graphics/API/Buffer.hpp>

namespace le::vk
{
    class Buffer final : public le::Buffer
    {
    public:
        void* GetMappedData() override;
        void* GetMappedDataForFrame(size_t currentFrame) override;

        VkBuffer GetBuffer() const;
    private:
        VkBuffer m_buffer = nullptr;
        VmaAllocation m_allocation = nullptr;
    };
}
