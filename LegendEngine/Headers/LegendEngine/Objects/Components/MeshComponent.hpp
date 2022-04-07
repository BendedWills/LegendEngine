#ifndef _LEGENDENGINE_MESHCOMPONENT_HPP
#define _LEGENDENGINE_MESHCOMPONENT_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Objects/Components/Component.hpp>

namespace LegendEngine::Objects::Components
{
    class MeshComponent : public Component, IDisposable
    {
    public:
        LEGENDENGINE_NO_COPY(MeshComponent);
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(MeshComponent);

        MeshComponent() {}

        bool Init(VertexTypes::Vertex2* pVertices, uint32_t vertexCount);

        VertexBuffer* GetVertexBuffer();
        uint32_t GetVertexCount();
    private:
        void OnDispose();

        Ref<VertexBuffer> vertexBuffer;
        uint32_t vertexCount = 0;
    };
}

#endif //_LEGENDENGINE_MESHCOMPONENT_HPP