#pragma once

#include <cstdint>
#include <LE/Common/UID.hpp>

namespace le
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Update() {}
        [[nodiscard]] virtual uint32_t GetWidth() const;
        [[nodiscard]] virtual uint32_t GetHeight() const;

        void SetCamera(UID camera);
        [[nodiscard]] UID GetCameraID() const;
    protected:
        UID m_cameraUID = 0;
    };
}
