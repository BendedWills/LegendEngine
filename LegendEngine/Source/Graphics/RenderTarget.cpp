#include <LegendEngine/Application.hpp>
#include <LegendEngine/Graphics/RenderTarget.hpp>

namespace LegendEngine::Graphics
{
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
