#ifndef _LEGENDENGINE_MESHCOMPONENT_HPP
#define _LEGENDENGINE_MESHCOMPONENT_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Objects/Components/Component.hpp>

namespace LegendEngine::Objects::Components
{
    class MeshComponent : public Component, IDisposable
    {
    public:
        bool Init(VertexTypes::Vertex2* pVertices, uint64_t vertexCount);

        VertexBuffer* GetVertexBuffer();
        uint64_t GetVertexCount();
    private:
        void OnDispose();

        Ref<VertexBuffer> vertexBuffer;
        uint64_t vertexCount = 0;
    };
}

#endif //_LEGENDENGINE_MESHCOMPONENT_HPP