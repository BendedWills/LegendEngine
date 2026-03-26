#include <LE/Graphics/RenderTarget.hpp>

namespace le
{
    uint32_t RenderTarget::GetWidth() const
    {
        return 100;
    }

    uint32_t RenderTarget::GetHeight() const
    {
        return 100;
    }

    void RenderTarget::SetCamera(UID cameraID)
    {
        m_cameraUID = cameraID;
    }

    UID RenderTarget::GetCameraID() const
    {
        return m_cameraUID;
    }
}
