#ifndef _LEGENDENGINE_VERTEXBUFFER_HPP
#define _LEGENDENGINE_VERTEXBUFFER_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/NativeHolder.hpp>
#include <LegendEngine/Common/IApplicationHolder.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <stdint.h>

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

	class VertexBuffer;
	class IVertexBufferNative
	{
	public:
		LEGENDENGINE_NO_COPY(IVertexBufferNative);

        IVertexBufferNative(VertexBuffer* pVertexBuffer)
			:
            pVertexBuffer(pVertexBuffer)
		{}

		virtual bool OnCreate(
			VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
            uint32_t* pIndices, uint64_t indexCount
        ) { return false; }
		virtual void OnDispose() {}
	private:
        VertexBuffer* pVertexBuffer = nullptr;
	};

    class VertexBuffer : 
        public IDisposable, 
        public NativeHolder<IVertexBufferNative>,
        public IApplicationHolder
    {
    public:
        LEGENDENGINE_NO_COPY(VertexBuffer);
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(VertexBuffer);

        VertexBuffer() = default;
        
        /**
         * @brief Initializes the vertex buffer.
         * 
         * @param pVertices An array of vertices.
         * @param vertexCount The amount of vertices in pVertices.
		 * @param pIndices An array of indices.
		 * @param vertexCount The amount of indices in pIndices.
         * 
         * @returns True if the vertex buffer was successfully initialized;
         *  otherwise, false.
         */
        bool Init(
            VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
            uint32_t* pIndices, uint64_t indexCount
        );
    private:
        void OnDispose();
    };
}

#endif //_LEGENDENGINE_VERTEXBUFFER_HPP