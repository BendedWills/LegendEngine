#pragma once
#include <LegendEngine/Objects/Camera.hpp>

namespace LegendEngine::Graphics
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Update() {}
        virtual uint32_t GetWidth() const;
        virtual uint32_t GetHeight() const;

        void SetCamera(Objects::Camera* camera);
        [[nodiscard]] Objects::Camera* GetCamera() const;
    protected:
        Objects::Camera* m_pCamera = nullptr;
    };
}
