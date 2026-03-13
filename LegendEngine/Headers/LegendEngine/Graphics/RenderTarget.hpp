#pragma once
#include <LegendEngine/Objects/Camera.hpp>

namespace le
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Update() {}
        virtual uint32_t GetWidth() const;
        virtual uint32_t GetHeight() const;

        void SetCamera(Camera* camera);
        [[nodiscard]] Camera* GetCamera() const;
    protected:
        Camera* m_pCamera = nullptr;
    };
}
