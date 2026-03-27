#pragma once

#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Graphics/API/Image.hpp>
#include <LE/Graphics/API/Types.hpp>

namespace le
{
    class CommandBuffer
    {
    public:
        struct BufferImageCopy
        {
            size_t bufferOffset;
            uint32_t bufferRowLength;
            uint32_t bufferImageHeight;
            Image::Subresource imageSubresource;
            Offset3D imageOffset;
            Extent3D imageExtent;
        };

        struct ImageMemoryBarrier
        {
            Image::Layout oldLayout;
            Image::Layout newLayout;
            Image* image;
            Image::Subresource subresourceRange;
            AccessFlagBits srcAccessMask;
            AccessFlagBits dstAccessMask;
        };

        virtual ~CommandBuffer() = default;

        // Helper functions
        virtual void TransitionImageLayout(Image& image, Image::Format format,
            Image::Layout oldLayout, Image::Layout newLayout) = 0;

        virtual void Begin(bool oneTimeSubmit) = 0;
        virtual void End() = 0;
        virtual void Submit(bool wait) = 0;

        virtual void CmdCopyBufferToImage(Buffer& buffer, Image& image,
            Image::Layout layout, size_t count, BufferImageCopy* regions) = 0;
        virtual void CmdPipelineBarrier(PipelineStage srcStage, PipelineStage dstStage,
            size_t imageMemoryBarrierCount, ImageMemoryBarrier* imageMemoryBarriers) = 0;
    };
}
