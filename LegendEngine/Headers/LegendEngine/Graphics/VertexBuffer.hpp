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
        struct Vertex3c
        {
            float position[3];
			float color[3];
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

		virtual bool OnCreate(VertexTypes::Vertex3c* pVertices,
			uint64_t vertexCount) { return false; }
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
         * 
         * @returns True if the vertex buffer was successfully initialized;
         *  otherwise, false.
         */
        bool Init(VertexTypes::Vertex3c* pVertices, uint64_t vertexCount);
    private:
        void OnDispose();
    };
}

#endif //_LEGENDENGINE_VERTEXBUFFER_HPP