#pragma once

#include <LegendEngine/Components/Component.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>

#include <cstdint>
#include <LegendEngine/Resources/Material.hpp>

namespace le
{
    class MeshComponent : public Component
    {
    public:
        MeshComponent(std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices);
        MeshComponent(size_t initialVertexCount, size_t initialIndexCount);

        void UpdateMesh(std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices) const;

        void SetMaterial(Material* pMaterial);
        Material* GetMaterial() const;
        VertexBuffer& GetVertexBuffer() const;
        uint32_t GetIndexCount() const;
    private:
        Material* m_pMaterial = nullptr;
        Scope<VertexBuffer> m_VertexBuffer;
        uint64_t m_IndexCount = 0;
    };
}
