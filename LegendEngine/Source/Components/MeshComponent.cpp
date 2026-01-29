#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>

namespace LegendEngine::Components
{
    MeshComponent::MeshComponent(VertexTypes::Vertex3* pVertices,
        const uint64_t vertexCount, uint32_t* pIndices, const uint64_t indexCount)
        :
        m_VertexBuffer(Application::Get().GetGraphicsContext().CreateVertexBuffer(
            pVertices, vertexCount, pIndices, indexCount))
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

    uint32_t MeshComponent::GetVertexCount() const
    {
        return m_VertexCount;
    }
}
