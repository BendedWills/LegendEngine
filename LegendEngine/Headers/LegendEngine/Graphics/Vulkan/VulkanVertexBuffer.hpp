#pragma once

#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace le
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        // Called on rendering, by the main thread
        virtual void DeleteUnusedBuffers(const std::vector<VkFence>& fences, uint32_t currentFrame) {}

        virtual VkBuffer GetVertexBuffer() const = 0;
        virtual VkBuffer GetIndexBuffer() const = 0;
    };
}
