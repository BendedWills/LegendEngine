#ifndef _LEGENDENGINE_MESHCOMPONENT_HPP
#define _LEGENDENGINE_MESHCOMPONENT_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Object3d/Components/Component.hpp>

namespace LegendEngine::Object3d::Components
{
    class MeshComponent : public Component, IDisposable
    {
    public:
        bool Init(VertexTypes::Vertex2* pVertices, uint64_t vertexCount);
    private:
        void OnDispose();

        Ref<VertexBuffer> vertexBuffer;
    };
}

#endif //_LEGENDENGINE_MESHCOMPONENT_HPP