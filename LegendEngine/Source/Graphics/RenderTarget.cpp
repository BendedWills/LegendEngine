#include <LegendEngine/Application.hpp>
#include <LegendEngine/Graphics/RenderTarget.hpp>

namespace LegendEngine::Graphics
{
    RenderTarget::RenderTarget(Scope<RenderTargetBridge> bridge)
        :
        m_Bridge(std::move(bridge))
    {}

    RenderTargetBridge& RenderTarget::GetBridge() const
    {
        return *m_Bridge;
    }

    bool RenderTarget::IsCloseRequested() const
    {
        Application::Get().GetLogger().Log(Logger::Level::WARN,
            "RenderTarget::IsCloseRequested called but it was not "
            "implemented. Is the application running headless?");
        return false;
    }

    uint32_t RenderTarget::GetWidth() const
    {
        return 100;
    }

    uint32_t RenderTarget::GetHeight() const
    {
        return 100;
    }

    void RenderTarget::SetCamera(Objects::Camera* camera)
    {
        m_pCamera = camera;
    }

    Objects::Camera* RenderTarget::GetCamera() const
    {
        return m_pCamera;
    }
}
