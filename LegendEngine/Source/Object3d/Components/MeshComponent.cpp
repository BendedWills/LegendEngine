#include <LegendEngine/Object3d/Components/MeshComponent.hpp>
#include <LegendEngine/Object3d/Object.hpp>
#include <LegendEngine/Application3D.hpp>

using namespace LegendEngine::Object3d::Components;

bool MeshComponent::Init(VertexTypes::Vertex2* pVertices, uint64_t vertexCount)
{
    if (GetObject()->GetApplication() == nullptr)
        return false;
    
    return vertexBuffer.Init(GetObject()->GetApplication()->GetRenderer(),
        pVertices, vertexCount);
}

void MeshComponent::OnDispose()
{
    
}