#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>

namespace LegendEngine::Components
{
    MeshComponent::MeshComponent(const std::span<VertexTypes::Vertex3> vertices,
            const std::span<uint32_t> indices)
        :
        m_VertexBuffer(Application::Get().GetGraphicsContext().CreateVertexBuffer(
            vertices, indices)),
        m_IndexCount(indices.size())
    {}

    void MeshComponent::SetMaterial(Resources::Material* pMaterial)
    {
        m_pMaterial = pMaterial;
    }

    Resources::Material* MeshComponent::GetMaterial() const
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
