#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Objects/Object.hpp>

#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>

namespace LegendEngine
{
    IRenderer::IRenderer(Application& application)
		:
		m_Application(application)
    {
		m_Application.Log("Initializing renderer", LogType::DEBUG);

        if (!OnRendererInit())
            throw std::runtime_error("Failed to initialize renderer");

		m_Application.Log("Initialized renderer", LogType::DEBUG);
    }

	IRenderer::~IRenderer()
	{
		m_Application.Log("Disposing renderer", LogType::DEBUG);
		{
			OnRendererDispose();
		}
		m_Application.Log("Disposed renderer", LogType::DEBUG);
	}

	Application* IRenderer::GetApplication()
	{
		return &m_Application;
	}
}
