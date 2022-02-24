#include <LegendEngine/Object3d/Components/MeshComponent.hpp>
#include <LegendEngine/Object3d/Object.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Application3D.hpp>

#include <sstream>

using namespace LegendEngine::Object3d::Components;

bool MeshComponent::Init(VertexTypes::Vertex2* pVertices, uint64_t vertexCount)
{
    if (GetObject() == nullptr)
        return false;
    if (GetObject()->GetApplication() == nullptr)
        return false;

    Application3D* pApplication = GetObject()->GetApplication();
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

    return true;
}

void MeshComponent::OnDispose()
{
    vertexBuffer->Dispose();
}