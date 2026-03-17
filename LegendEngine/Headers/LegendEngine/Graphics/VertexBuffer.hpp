#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <span>

namespace le
{
    namespace VertexTypes
    {
        struct Vertex3
        {
            float position[3];
			float texcoord[3];
		};
    }

    class VertexBuffer
    {
    public:
        enum class UpdateFrequency
        {
            UPDATES_ONCE,
            UPDATES_OCCASIONALLY,
            UPDATES_OFTEN,
        };

        LEGENDENGINE_NO_COPY(VertexBuffer);

    	VertexBuffer() = default;
        virtual ~VertexBuffer() = 0;

        virtual void Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices) = 0;
        virtual void Resize(size_t vertexCount, size_t indexCount) = 0;

        virtual size_t GetVertexCount() = 0;
        virtual size_t GetIndexCount() = 0;
    };
}
