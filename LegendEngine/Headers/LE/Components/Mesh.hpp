#pragma once

#include <cstdint>
#include <span>

#include <LE/Graphics/VertexBuffer.hpp>
#include <LE/Components/Component.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Common/Types.hpp>

namespace le
{
    class Mesh : public Component
    {
    public:
        using UpdateFrequency = VertexBuffer::UpdateFrequency;

        Mesh(std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices, UpdateFrequency frequency);
        Mesh(size_t initialVertexCount, size_t initialIndexCount,
            UpdateFrequency frequency);

        void UpdateMesh(std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices);

        void SetMaterial(Material* pMaterial);
        [[nodiscard]] Material* GetMaterial() const;
        [[nodiscard]] Ref<VertexBuffer> GetVertexBuffer() const;
        [[nodiscard]] uint32_t GetIndexCount() const;
    private:
        Material* m_pMaterial = nullptr;
        Ref<VertexBuffer> m_VertexBuffer;
        uint64_t m_IndexCount = 0;
    };
}
