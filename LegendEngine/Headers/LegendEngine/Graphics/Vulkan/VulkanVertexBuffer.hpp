#pragma once

#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace le
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        struct BufferInfo
        {
            VkBuffer vertex = nullptr;
            VkBuffer index = nullptr;
            size_t indexCount = 0;
        };

        // Called on rendering, by the main thread
        virtual void DeleteUnusedBuffers(uint32_t currentFrame) {}
        virtual void Use(uint32_t currentFrame) {}

        virtual BufferInfo GetBufferInfo() const = 0;

        virtual bool ShouldWait() { return false; }
        virtual void DeleteStager() {}
        virtual size_t StartWait() { return 0; }

        virtual VkSemaphore GetSemaphore() const { return nullptr; }
    };
}
