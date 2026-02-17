#pragma once

#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <Tether/Window.hpp>

namespace LegendEngine::Graphics
{
    class WindowRenderTarget final : public RenderTarget
    {
    public:
        WindowRenderTarget(GraphicsContext& ctx, Tether::Window& window);

        bool IsCloseRequested() const override;

        [[nodiscard]] Tether::Window& GetWindow() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
    private:
        void ReceiveResize(uint64_t width, uint64_t height) const;

        class EventHandler final : public Tether::Events::EventHandler
        {
        public:
            explicit EventHandler(WindowRenderTarget& parent);
            void OnWindowResize(Tether::Events::WindowResizeEvent event) const;
        private:
            WindowRenderTarget& m_Parent;
        };
        EventHandler m_EventHandler;

        Tether::Window& m_Window;
    };
}
