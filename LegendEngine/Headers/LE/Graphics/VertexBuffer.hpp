#pragma once

#include <span>
#include <LE/Common/Defs.hpp>

namespace le
{
    class VertexBuffer
    {
    public:
        enum class UpdateFrequency
        {
            UPDATES_ONCE,
            UPDATES_OCCASIONALLY,
            UPDATES_OFTEN,
        };

        LE_NO_COPY(VertexBuffer);

    	VertexBuffer() = default;
        virtual ~VertexBuffer() = 0;

        virtual void Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices) = 0;
        virtual void Resize(size_t vertexCount, size_t indexCount) = 0;

        virtual size_t GetVertexCount() = 0;
        virtual size_t GetIndexCount() = 0;
    };
}
