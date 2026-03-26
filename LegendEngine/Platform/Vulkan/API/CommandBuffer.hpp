#pragma once

#include <LE/Graphics/API/CommandBuffer.hpp>
#include <vulkan/vulkan.h>

namespace le::vk
{
    class CommandBuffer final : public le::CommandBuffer
    {
    public:
        void TransitionImageLayout(Image& image, Image::Format format, Image::Layout oldLayout,
            Image::Layout newLayout) override;

        void Begin(bool oneTimeSubmit) override;

        void End() override;

        void Submit(bool wait) override;

        void CmdCopyBufferToImage(Buffer& buffer, Image& image, Image::Layout layout, size_t count,
            BufferImageCopy* regions) override;
    private:
        VkCommandBuffer m_commandBuffer = nullptr;
    };
}
