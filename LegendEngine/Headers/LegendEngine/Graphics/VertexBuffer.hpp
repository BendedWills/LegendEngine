#ifndef _LEGENDENGINE_VERTEXBUFFER_HPP
#define _LEGENDENGINE_VERTEXBUFFER_HPP

#include <LegendEngine/Common/Types.hpp>
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
            float color[3];
        };
    }

    class VertexBuffer : public IDisposable
    {
    public:
        ~VertexBuffer()
        {
            Dispose();
        }

        VertexBuffer(IRenderer* pRenderer, RenderingAPI type)
            :
            pRenderer(pRenderer),
            type(type)
        {}
        
        VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) = delete;

        /**
         * @brief Initializes the vertex buffer.
         * 
         * @param pVertices An array of vertices.
         * @param vertexCount The amount of vertices in pVertices.
         * 
         * @returns True if the vertex buffer was successfully initialized;
         *  otherwise, false.
         */
        bool Init(VertexTypes::Vertex2* pVertices, uint64_t vertexCount);

        RenderingAPI GetType();
    protected:
        void OnDispose();

        virtual bool OnBufferCreate(VertexTypes::Vertex2* pVertices, 
            uint64_t vertexCount) { return false; }
        virtual void OnBufferDispose() {}

        IRenderer* pRenderer = nullptr;
        RenderingAPI type;
    };
}

#endif //_LEGENDENGINE_VERTEXBUFFER_HPP