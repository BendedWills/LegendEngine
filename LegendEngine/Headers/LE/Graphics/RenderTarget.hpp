#pragma once

#include <cstdint>

import le.camera;

namespace le
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Update() {}
        [[nodiscard]] virtual uint32_t GetWidth() const;
        [[nodiscard]] virtual uint32_t GetHeight() const;

        void SetCamera(Camera* camera);
        [[nodiscard]] Camera* GetCamera() const;
    protected:
        Camera* m_pCamera = nullptr;
    };
}
