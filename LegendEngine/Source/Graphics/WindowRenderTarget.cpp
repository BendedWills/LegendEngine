#include <LegendEngine/Graphics/GraphicsContext.hpp>
#include <LegendEngine/Graphics/WindowRenderTarget.hpp>

namespace LegendEngine::Graphics
{
    WindowRenderTarget::WindowRenderTarget(GraphicsContext& ctx,
        Tether::Window& window)
        :
        RenderTarget(ctx.CreateWindowRenderTargetBridge(window)),
        m_EventHandler(*this),
        m_Window(window)
    {
        m_Window.AddEventHandler(m_EventHandler, Tether::Events::EventType::WINDOW_RESIZE);
    }

    bool WindowRenderTarget::IsCloseRequested() const
    {
        return m_Window.IsCloseRequested();
    }

    Tether::Window& WindowRenderTarget::GetWindow() const
    {
        return m_Window;
    }

    uint32_t WindowRenderTarget::GetWidth() const
    {
        return m_Window.GetWidth();
    }

    uint32_t WindowRenderTarget::GetHeight() const
    {
        return m_Window.GetHeight();
    }

    void WindowRenderTarget::ReceiveResize(uint64_t width, uint64_t height) const
    {
        if (!m_pCamera)
            return;

        const float aspect = static_cast<float>(m_Window.GetWidth()) /
            static_cast<float>(m_Window.GetHeight());
        m_pCamera->SetAspectRatio(aspect);
    }

    WindowRenderTarget::EventHandler::EventHandler(WindowRenderTarget& parent)
        :
        m_Parent(parent)
    {}

    void WindowRenderTarget::EventHandler::OnWindowResize(const Tether::Events::WindowResizeEvent event) const
    {
        if (m_Parent.m_Window.IsVisible())
            m_Parent.ReceiveResize(event.GetNewWidth(), event.GetNewHeight());
    }
}
