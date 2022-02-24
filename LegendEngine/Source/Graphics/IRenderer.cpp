#include <LegendEngine/Graphics/IRenderer.hpp>

#include <LegendEngine/Application3D.hpp>

using namespace LegendEngine;

bool IRenderer::Init(IApplication* pApplication)
{
    if (initialized || !pApplication)
        return false;
    
    this->pApplication = pApplication;

    pApplication->Log("Initializing renderer", LogType::DEBUG);

    if (!OnRendererInit())
    {
        pApplication->Log("Failed to initialize renderer", LogType::ERROR);
        return false;
    }
    else
        pApplication->Log("Initialized renderer", LogType::DEBUG);

    initialized = true;
    return true;
}

IApplication* IRenderer::GetApplication()
{
    return pApplication;
}

void IRenderer::OnDispose()
{
    pApplication->Log("Disposing renderer", LogType::DEBUG);
    OnRendererDispose();
    pApplication->Log("Disposed renderer", LogType::DEBUG);
}
