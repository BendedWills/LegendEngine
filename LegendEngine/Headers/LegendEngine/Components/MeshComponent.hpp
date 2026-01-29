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
        MeshComponent(std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices);

        void SetMaterial(Resources::Material* pMaterial);
        Resources::Material* GetMaterial() const;
        VertexBuffer& GetVertexBuffer() const;
        uint32_t GetIndexCount() const;
    private:
        Resources::Material* m_pMaterial = nullptr;
        Scope<VertexBuffer> m_VertexBuffer;
        uint64_t m_IndexCount = 0;
    };
}
