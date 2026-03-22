#include <LE/Camera.hpp>
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

    void RenderTarget::SetCamera(Camera* camera)
    {
        m_pCamera = camera;
    }

    Camera* RenderTarget::GetCamera() const
    {
        return m_pCamera;
    }
}
