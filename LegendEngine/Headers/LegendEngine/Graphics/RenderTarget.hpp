#pragma once
#include <LegendEngine/Objects/Camera.hpp>

namespace LegendEngine::Graphics
{
    class RenderTargetBridge
    {};

    class RenderTarget
    {
    public:
        explicit RenderTarget(Scope<RenderTargetBridge> bridge);
        virtual ~RenderTarget() = default;

        [[nodiscard]] virtual bool IsCloseRequested() const;
        virtual void Update() {}
        virtual void SetVisible(bool visible) {}
        virtual uint32_t GetWidth() const;
        virtual uint32_t GetHeight() const;

        // If the render target is resized, this camera's aspect ratio will be
        // updated accordingly.
        void SetCamera(Objects::Camera* camera);
        [[nodiscard]] Objects::Camera* GetCamera() const;

        [[nodiscard]] RenderTargetBridge& GetBridge() const;
    protected:
        Objects::Camera* m_pCamera = nullptr;
    private:
        const Scope<RenderTargetBridge> m_Bridge;
    };
}
