#pragma once

#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class MeshData : public Resource
    {
    public:
        enum class UpdateFrequency
        {
            UPDATES_ONCE,
            UPDATES_OCCASIONALLY,
            UPDATES_OFTEN,
        };

        struct Vertex3
        {
            float position[3];
            float texcoord[3];
        };

        MeshData(std::span<Vertex3> vertices, std::span<uint32_t> indices,
            UpdateFrequency updateFrequency);
        MeshData(size_t initialVertexCount, size_t initialIndexCount,
            UpdateFrequency frequency);

        ID<MeshData> id = ID<MeshData>(m_uid);
    private:
        Scope<Buffer> m_vertexBuffer;
        Scope<Buffer> m_indexBuffer;

        size_t m_vertexCount;
        size_t m_indexCount;
    };
}
