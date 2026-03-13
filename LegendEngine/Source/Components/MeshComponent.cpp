#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>

namespace le
{
    MeshComponent::MeshComponent(const std::span<VertexTypes::Vertex3> vertices,
        const std::span<uint32_t> indices)
        :
        m_VertexBuffer(Application::Get().GetGraphicsContext().CreateVertexBuffer(
            vertices.size(), indices.size())),
        m_IndexCount(indices.size())
    {
        UpdateMesh(vertices, indices);
    }

    MeshComponent::MeshComponent(size_t initialVertexCount, size_t initialIndexCount)
        :
        m_VertexBuffer(Application::Get().GetGraphicsContext().CreateVertexBuffer(
            initialVertexCount, initialIndexCount)),
        m_IndexCount(initialIndexCount)
    {}

    void MeshComponent::UpdateMesh(const std::span<VertexTypes::Vertex3> vertices,
        const std::span<uint32_t> indices) const
    {
        m_VertexBuffer->Update(vertices, indices);
    }

    void MeshComponent::SetMaterial(Material* pMaterial)
    {
        m_pMaterial = pMaterial;
    }

    Material* MeshComponent::GetMaterial() const
    {
        return m_pMaterial;
    }

    VertexBuffer& MeshComponent::GetVertexBuffer() const
    {
        return *m_VertexBuffer;
    }

    uint32_t MeshComponent::GetIndexCount() const
    {
        return m_IndexCount;
    }
}
