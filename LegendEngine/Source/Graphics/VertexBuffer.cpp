#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;

bool VertexBuffer::Init(VertexTypes::Vertex2* pVertices, 
    uint64_t vertexCount)
{
    if (initialized)
        return false;
    
    pRenderer->vertexBuffers.push_back(this);
    if (nativeSet)
        if (!native->OnCreate(pVertices, vertexCount))
            return false;
    
    initialized = true;
    return true;
}

void VertexBuffer::OnDispose()
{
    if (nativeSet)
        native->OnDispose();

    Tether::VectorUtils::EraseAll(pRenderer->vertexBuffers, this);

    LEGENDENGINE_OBJECT_LOG(
        pRenderer->GetApplication(), "VertexBuffer", 
        "Disposed vertex buffer", 
        LogType::DEBUG
    );
}