#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;

bool VertexBuffer::Init(
	VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
    uint32_t* pIndices, uint64_t indexCount
)
{
    if (initialized)
        return false;
    
    pApplication->vertexBuffers.push_back(this);
    if (nativeSet)
        if (!native->OnCreate(pVertices, vertexCount, pIndices, indexCount))
            return false;
    
    initialized = true;
    return true;
}

void VertexBuffer::OnDispose()
{
    if (nativeSet)
        native->OnDispose();

    Tether::VectorUtils::EraseAll(pApplication->vertexBuffers, this);

    LEGENDENGINE_OBJECT_LOG(
        pApplication, "VertexBuffer", 
        "Disposed vertex buffer", 
        LogType::DEBUG
    );
}