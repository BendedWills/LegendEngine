#pragma once

#include "API/Buffer.hpp"

namespace le::vk
{
    class PerFrameBuffer final : public Buffer
    {
    public:
        PerFrameBuffer(Usage usage, size_t size);

        void Update(size_t size, const void* data) override;

        void UpdateForAllFrames(size_t size, void* data) override;

        VkBuffer GetBuffer() override;
    };
}
