#pragma once

#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <span>
#include <vulkan/vulkan.h>

namespace le
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(
            Tether::Rendering::Vulkan::GraphicsContext& context,
            size_t initialVertexCount, size_t initialIndexCount
        );
        ~VulkanVertexBuffer() override = default;

        void Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices) override;
        void Resize(size_t vertexBufferSize, size_t indexCount) override;
        size_t GetVertexCount() override;
        size_t GetIndexCount() override;

        VkBuffer GetVertexBuffer();
        VkBuffer GetIndexBuffer();
    private:
        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        std::optional<Tether::Rendering::Vulkan::VertexBuffer> m_VertexBuffer;

        size_t m_VertexCount = 0;
    };
}
