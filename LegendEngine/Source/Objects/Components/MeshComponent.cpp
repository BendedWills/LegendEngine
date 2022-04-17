#include <LegendEngine/Objects/Components/MeshComponent.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Application.hpp>

#include <sstream>

using namespace LegendEngine::Objects::Components;

bool MeshComponent::Init(VertexTypes::Vertex3c* pVertices, uint32_t vertexCount)
{
    if (initialized)
        return false;
    
    if (GetObject() == nullptr)
        return false;
    if (GetObject()->GetApplication() == nullptr)
        return false;
    
    Application* pApplication = GetObject()->GetApplication();
    IRenderer* pRenderer = pApplication->GetRenderer();
    
    if (pRenderer == nullptr)
    {
        LEGENDENGINE_OBJECT_LOG(
            pApplication, "MeshComponent",
            "Renderer is nullptr!",
            LogType::ERROR
        );

        return false;
    }

    vertexBuffer = RefTools::Create<VertexBuffer>(pRenderer);
    if (!pRenderer->CreateVertexBufferNative(vertexBuffer.get()))
    {
        LEGENDENGINE_OBJECT_LOG(
            pApplication, "MeshComponent",
            "Failed to create vertex buffer!",
            LogType::ERROR
        );
        
        return false;
    }

    if (!vertexBuffer->Init(pVertices, vertexCount))
    {
        LEGENDENGINE_OBJECT_LOG(
            pApplication, "MeshComponent",
            "Failed to init vertex buffer!",
            LogType::ERROR
        );

        return false;
    }

    LEGENDENGINE_OBJECT_LOG(
        pApplication, "MeshComponent",
        "Finished initialization.",
        LogType::DEBUG
    );

    this->vertexCount = vertexCount;

    initialized = true;
    return true;
}

LegendEngine::VertexBuffer* MeshComponent::GetVertexBuffer()
{
    return vertexBuffer.get();
}

uint32_t MeshComponent::GetVertexCount()
{
    return vertexCount;
}

void MeshComponent::OnDispose()
{
	vertexBuffer->Dispose();
}