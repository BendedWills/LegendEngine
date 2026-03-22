#include <LE/Application.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Graphics/GraphicsContext.hpp>

namespace le
{
    Mesh::Mesh(const std::span<VertexTypes::Vertex3> vertices,
        const std::span<uint32_t> indices, UpdateFrequency updateFrequency)
        :
        m_VertexBuffer(Application::Get().GetGraphicsContext().CreateVertexBuffer(
            vertices.size(), indices.size(), updateFrequency)),
        m_IndexCount(indices.size())
    {
        UpdateMesh(vertices, indices);
    }

    Mesh::Mesh(const size_t initialVertexCount, const size_t initialIndexCount,
        const UpdateFrequency frequency)
        :
        m_VertexBuffer(Application::Get().GetGraphicsContext().CreateVertexBuffer(
            initialVertexCount, initialIndexCount, frequency)),
        m_IndexCount(initialIndexCount)
    {}

    void Mesh::UpdateMesh(const std::span<VertexTypes::Vertex3> vertices,
        const std::span<uint32_t> indices)
    {
        m_VertexBuffer->Update(vertices, indices);
        m_IndexCount = indices.size();
    }

    void Mesh::SetMaterial(Material* pMaterial)
    {
        m_pMaterial = pMaterial;
    }

    Material* Mesh::GetMaterial() const
    {
        return m_pMaterial;
    }

    Ref<VertexBuffer> Mesh::GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

    uint32_t Mesh::GetIndexCount() const
    {
        return m_IndexCount;
    }
}
