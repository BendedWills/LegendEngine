#ifndef _LEGENDENGINE_VERTEXBUFFER_HPP
#define _LEGENDENGINE_VERTEXBUFFER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <stdint.h>

namespace LegendEngine
{
    namespace VertexTypes
    {
        struct Vertex2
        {
            float position[2];
        };
    }

    class VertexBuffer : public IDisposable
    {
    public:
        VertexBuffer() {}
        
        VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) = delete;

        /**
         * @brief Initializes the vertex buffer.
         * 
         * @param pRenderer A pointer to the renderer
         * @param pVertices An array of vertices.
         * @param vertexCount The amount of vertices in pVertices.
         * 
         * @returns True if the vertex buffer was successfully initialized;
         *  otherwise, false.
         */
        bool Init(IRenderer* pRenderer, VertexTypes::Vertex2* pVertices, 
            uint64_t vertexCount);
    protected:
        void OnDispose();

        virtual bool OnBufferCreate(VertexTypes::Vertex2* pVertices, 
            uint64_t vertexCount) { return false; }
        virtual void OnBufferDispose() {}

        IRenderer* pRenderer = nullptr;
    };
}

#endif //_LEGENDENGINE_VERTEXBUFFER_HPP