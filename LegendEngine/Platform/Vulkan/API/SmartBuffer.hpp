#pragma once

#include "API/Buffer.hpp"

namespace le::vk
{
    class SmartBuffer final : public Buffer
    {
    public:
        SmartBuffer(Usage usage, size_t initialSize);

        void Update(size_t size, const void* data) override;

        void UpdateForAllFrames(size_t size, void* data) override;

        VkBuffer GetBuffer() override;
    };
}
