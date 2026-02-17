#include <LegendEngine/Graphics/Vulkan/VulkanRenderTargetBridge.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    VulkanRenderTargetBridge::VulkanRenderTargetBridge(TetherVulkan::GraphicsContext& context)
        :
        m_Surface(context),
        m_IsHeadless(true)
    {}

#ifndef LGENG_HEADLESS
    VulkanRenderTargetBridge::VulkanRenderTargetBridge(TetherVulkan::GraphicsContext& context,
        Tether::Window& window)
        :
        m_Surface(context, window),
        m_IsHeadless(false)
    {}
#endif

    VkSurfaceKHR VulkanRenderTargetBridge::GetSurface()
    {
        return m_Surface.Get();
    }

    bool VulkanRenderTargetBridge::IsHeadless() const
    {
        return m_IsHeadless;
    }
}
