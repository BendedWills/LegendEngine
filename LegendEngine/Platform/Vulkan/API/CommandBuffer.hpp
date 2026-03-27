#pragma once

#include <mutex>
#include <LE/Graphics/API/CommandBuffer.hpp>
#include <vulkan/vulkan.h>

namespace le::vk
{
    class CommandBuffer final : public le::CommandBuffer
    {
    public:
        CommandBuffer(VkQueue queue, std::mutex& mutex);

        void TransitionImageLayout(Image& image, Image::Format format, Image::Layout oldLayout,
            Image::Layout newLayout) override;

        void Begin(bool oneTimeSubmit) override;

        void End() override;

        void Submit(bool wait) override;

        void CmdCopyBufferToImage(le::Buffer& buffer, le::Image& image, Image::Layout layout, size_t count,
            BufferImageCopy* regions) override;

        void CmdPipelineBarrier(PipelineStage srcStage, PipelineStage dstStage, size_t imageMemoryBarrierCount,
            ImageMemoryBarrier* imageMemoryBarriers) override;
    private:
        VkCommandBuffer m_commandBuffer = nullptr;
    };
}
