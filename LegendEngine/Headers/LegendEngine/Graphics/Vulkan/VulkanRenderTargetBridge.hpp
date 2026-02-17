#pragma once

#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <Tether/Window.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanRenderTargetBridge final : public RenderTargetBridge
    {
    public:
        VulkanRenderTargetBridge(TetherVulkan::GraphicsContext& context); // Headless
        VulkanRenderTargetBridge(TetherVulkan::GraphicsContext& context, Tether::Window& window);
        LEGENDENGINE_NO_COPY(VulkanRenderTargetBridge);

        bool IsHeadless() const;
        VkSurfaceKHR GetSurface();
    private:
        TetherVulkan::Surface m_Surface;
        bool m_IsHeadless;
    };
}
