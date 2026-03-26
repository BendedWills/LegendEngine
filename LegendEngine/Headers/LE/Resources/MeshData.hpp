#pragma once

#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class MeshData : public Resource
    {
    public:
        struct Vertex3
        {
            float position[3];
            float texcoord[3];
        };

        MeshData();

        ID<MeshData> id = ID<MeshData>(m_uid);
    private:
        Scope<Buffer> m_vertexBuffer;
        Scope<Buffer> m_indexBuffer;
    };
}
