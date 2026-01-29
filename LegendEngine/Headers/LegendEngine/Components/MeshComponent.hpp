#pragma once

#include <LegendEngine/Components/Component.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>

#include <cstdint>
#include <LegendEngine/Resources/Material.hpp>

namespace LegendEngine::Components
{
    class MeshComponent : public Component
    {
    public:
        MeshComponent(VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
            uint32_t* pIndices, uint64_t indexCount);

        void SetMaterial(Resources::Material* pMaterial);
        Resources::Material* GetMaterial() const;
        VertexBuffer& GetVertexBuffer() const;
        uint32_t GetVertexCount() const;
    private:
        Resources::Material* m_pMaterial = nullptr;
        Scope<VertexBuffer> m_VertexBuffer;
        uint64_t m_VertexCount = 0;
    };
}
