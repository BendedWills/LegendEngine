#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;

bool VertexBuffer::Init(VertexTypes::Vertex2* pVertices, 
    uint64_t vertexCount)
{
    pRenderer->vertexBuffers.push_back(this);
    if (!OnBufferCreate(pVertices, vertexCount))
        return false;
    
    initialized = true;
    return true;
}

void VertexBuffer::OnDispose()
{
    OnBufferDispose();
    Tether::VectorUtils::EraseAll(pRenderer->vertexBuffers, this);
}