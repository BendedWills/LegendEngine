#include "API/CommandBuffer.hpp"

namespace le::vk
{
    void CommandBuffer::TransitionImageLayout(Image& image, Image::Format format, Image::Layout oldLayout,
        Image::Layout newLayout) {}

    void CommandBuffer::Begin(bool oneTimeSubmit) {}
    void CommandBuffer::End() {}
    void CommandBuffer::Submit(bool wait) {}

    void CommandBuffer::CmdCopyBufferToImage(Buffer& buffer, Image& image, Image::Layout layout, size_t count,
        BufferImageCopy* regions) {}
}
