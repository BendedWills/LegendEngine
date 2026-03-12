#include <LegendEngine/Graphics/Vulkan/VulkanRenderTarget.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    VulkanRenderTarget::VulkanRenderTarget(TetherVulkan::GraphicsContext& context)
        :
        m_Surface(context),
        m_IsHeadless(true)
    {}

#ifndef LE_HEADLESS
    VulkanRenderTarget::VulkanRenderTarget(TetherVulkan::GraphicsContext& context,
        Tether::Window& window)
        :
        m_Surface(context, window),
        m_IsHeadless(false)
    {}
#endif

    bool VulkanRenderTarget::IsHeadless() const
    {
        return m_IsHeadless;
    }

    VkSurfaceKHR VulkanRenderTarget::GetSurface()
    {
        return m_Surface.Get();
    }
}
