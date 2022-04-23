#include <LegendEngine/Objects/Components/MeshComponent.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Application.hpp>

#include <sstream>

using namespace LegendEngine;
using namespace LegendEngine::Objects::Components;

bool MeshComponent::Init(
	VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
	uint32_t* pIndices, uint64_t indexCount
)
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

    vertexBuffer = pApplication->CreateVertexBuffer();
    if (!vertexBuffer->Init(pVertices, vertexCount, pIndices, indexCount))
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

    this->vertexCount = indexCount;

    initialized = true;
    return true;
}

void MeshComponent::SetMaterial(Resources::Material* pMaterial)
{
    this->pMaterial = pMaterial;
}

Resources::Material* MeshComponent::GetMaterial()
{
    return pMaterial;
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