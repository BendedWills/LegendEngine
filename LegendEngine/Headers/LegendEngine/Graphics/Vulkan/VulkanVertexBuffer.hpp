#pragma once

#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <span>
#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(
            Tether::Rendering::Vulkan::GraphicsContext& context,
            std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices
        );

        VkBuffer GetVertexBuffer();
        VkBuffer GetIndexBuffer();
    private:
        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        Tether::Rendering::Vulkan::VertexBuffer m_VertexBuffer;
    };
}
