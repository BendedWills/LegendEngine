#include <LegendEngine/Common/Assert.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>

namespace le
{
    using namespace Tether::Rendering::Vulkan;

    VulkanVertexBuffer::VulkanVertexBuffer(
            GraphicsContext& context,
            size_t initialVertexCount, size_t initialIndexCount
        )
        :
        m_Context(context),
        m_VertexCount(initialVertexCount)
    {
        m_VertexBuffer.emplace(context, initialVertexCount * sizeof(VertexTypes::Vertex3), initialIndexCount);
    }

    void VulkanVertexBuffer::Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices)
    {
        LE_ASSERT(vertices.size() * sizeof(VertexTypes::Vertex3) <= m_VertexBuffer->GetVertexBufferSize(), "Vertex buffer size mismatch");
        LE_ASSERT(indices.size() <= m_VertexBuffer->GetVertexCount(), "Vertex buffer size mismatch");

        m_VertexBuffer->UploadData(vertices.data(), indices.data());
        m_VertexBuffer->FinishDataUpload();
    }

    void VulkanVertexBuffer::Resize(size_t vertexBufferSize, size_t indexCount)
    {
        m_VertexBuffer.emplace(m_Context, vertexBufferSize, indexCount);
    }

    size_t VulkanVertexBuffer::GetVertexCount()
    {
        return m_VertexCount;
    }

    size_t VulkanVertexBuffer::GetIndexCount()
    {
        return m_VertexBuffer->GetVertexCount();
    }

    VkBuffer VulkanVertexBuffer::GetVertexBuffer()
    {
        return m_VertexBuffer->GetBuffer();
    }

    VkBuffer VulkanVertexBuffer::GetIndexBuffer()
    {
        return m_VertexBuffer->GetIndexBuffer();
    }
}