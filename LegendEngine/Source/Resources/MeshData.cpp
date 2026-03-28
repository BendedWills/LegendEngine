#include <LE/Application.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    MeshData::MeshData(const std::span<Vertex3> vertices, const std::span<uint32_t> indices, UpdateFrequency frequency)
        :
        m_vertexCount(vertices.size()),
        m_indexCount(indices.size())
    {
        GraphicsContext& context = Application::Get().GetGraphicsContext();
    }

    MeshData::MeshData(const size_t initialVertexCount, const size_t initialIndexCount, UpdateFrequency frequency)
        :
        m_vertexCount(initialVertexCount),
        m_indexCount(initialIndexCount)
    {
        GraphicsContext& context = Application::Get().GetGraphicsContext();

        m_vertexBuffer = context.CreateSimpleBuffer(Buffer::Usage::VERTEX_BUFFER,
            initialVertexCount * sizeof(Vertex3), false);
        m_indexBuffer = context.CreateSimpleBuffer(Buffer::Usage::INDEX_BUFFER,
            initialIndexCount * sizeof(uint32_t), false);
    }

    void MeshData::Update(std::span<Vertex3> vertices, std::span<uint32_t> indices)
    {

    }

    void MeshData::Resize(size_t vertexCount, size_t indexCount)
    {

    }

    size_t MeshData::GetVertexCount() const
    {
        return m_vertexCount;
    }

    size_t MeshData::GetIndexCount() const
    {
        return m_indexCount;
    }

    Buffer& MeshData::GetVertexBuffer() const
    {
        return *m_vertexBuffer;
    }

    Buffer& MeshData::GetIndexBuffer() const
    {
        return *m_indexBuffer;
    }
}
