#pragma once

#include <LegendEngine/Common/Defs.hpp>

namespace LegendEngine
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
        LEGENDENGINE_NO_COPY(VertexBuffer);

    	VertexBuffer() = default;
        virtual ~VertexBuffer() = 0;
    };
}
