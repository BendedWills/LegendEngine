#include <LegendEngine/Objects/Components/MeshComponent.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Application.hpp>

#include <sstream>

using namespace LegendEngine::Objects::Components;

bool MeshComponent::Init(VertexTypes::Vertex2* pVertices, uint64_t vertexCount)
{
    if (initialized)
        return false;
    
    if (GetObject() == nullptr)
        return false;
    if (GetObject()->GetApplication() == nullptr)
        return false;

    Application* pApplication = GetObject()->GetApplication();
    IRenderer* pRenderer = pApplication->GetRenderer();
    
    LEGENDENGINE_OBJECT_LOG(
        pApplication, "MeshComponent",
        "Found application and object. Checking for renderer...",
        LogType::DEBUG
    );

    if (pRenderer == nullptr)
    {
        LEGENDENGINE_OBJECT_LOG(
            pApplication, "MeshComponent",
            "Renderer is nullptr!",
            LogType::ERROR
        );

        return false;
    }

    LEGENDENGINE_OBJECT_LOG(
        pApplication, "MeshComponent",
        "Found renderer. Creating vertex buffer...",
        LogType::DEBUG
    );

    if (!pRenderer->CreateVertexBuffer(&vertexBuffer))
    {
        LEGENDENGINE_OBJECT_LOG(
            pApplication, "MeshComponent",
            "Failed to create vertex buffer!",
            LogType::ERROR
        );
        
        return false;
    }

    LEGENDENGINE_OBJECT_LOG(
        pApplication, "MeshComponent",
        "Created vertex buffer. Initializing vertex buffer...",
        LogType::DEBUG
    );
    
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

uint64_t MeshComponent::GetVertexCount()
{
    return vertexCount;
}

void MeshComponent::OnDispose()
{
    vertexBuffer->Dispose();
}