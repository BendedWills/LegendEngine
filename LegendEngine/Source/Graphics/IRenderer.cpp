#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Objects/Object.hpp>

#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>

using namespace LegendEngine;

bool IRenderer::Init(Application* pApplication)
{
    if (initialized || !pApplication)
        return false;

    if (!pApplication->IsInitialized())
        return false;
    
    this->pApplication = pApplication;

    pApplication->Log("Initializing renderer", LogType::DEBUG);

    if (!OnRendererInit())
    {
        pApplication->Log("Failed to initialize renderer", LogType::ERROR);
        return false;
    }

    initialized = true;

	defaultMaterial = pApplication->CreateResource<Resources::Material>();
	defaultMaterial->Init();

    pApplication->Log("Initialized renderer", LogType::DEBUG);

    return true;
}

Application* IRenderer::GetApplication()
{
    return pApplication;
}

void IRenderer::OnDispose()
{
    pApplication->Log("Disposing renderer", LogType::DEBUG);
    {
        defaultMaterial->Dispose();
        OnRendererDispose();
    }
    pApplication->Log("Disposed renderer", LogType::DEBUG);
}
