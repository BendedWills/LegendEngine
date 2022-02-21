#include <LegendEngine/Graphics/VertexBuffer.hpp>

using namespace LegendEngine;

bool VertexBuffer::Init(IRenderer* pRenderer, VertexTypes::Vertex2* pVertices, 
    uint64_t vertexCount)
{
    this->pRenderer = pRenderer;
    return OnBufferCreate(pVertices, vertexCount);
}

void VertexBuffer::OnDispose()
{
    OnBufferDispose();
}