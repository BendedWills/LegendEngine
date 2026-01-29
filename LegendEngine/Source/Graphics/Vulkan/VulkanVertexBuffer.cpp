#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Tether::Rendering::Vulkan;

    VulkanVertexBuffer::VulkanVertexBuffer(
            GraphicsContext& context,
            const std::span<VertexTypes::Vertex3> vertices,
            const std::span<uint32_t> indices
        )
        :
        m_Context(context),
        m_VertexBuffer(context, vertices.size() * sizeof(VertexTypes::Vertex3), indices.size())
    {
        m_VertexBuffer.UploadData(vertices.data(), indices.data());
		m_VertexBuffer.FinishDataUpload();
    }

    VkBuffer VulkanVertexBuffer::GetVertexBuffer()
    {
        return m_VertexBuffer.GetBuffer();
    }

    VkBuffer VulkanVertexBuffer::GetIndexBuffer()
    {
        return m_VertexBuffer.GetIndexBuffer();
    }
}