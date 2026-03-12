#pragma once

#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <Tether/Window.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanRenderTarget final : public RenderTarget
    {
    public:
        VulkanRenderTarget(TetherVulkan::GraphicsContext& context); // Headless
        VulkanRenderTarget(TetherVulkan::GraphicsContext& context, Tether::Window& window);
        LEGENDENGINE_NO_COPY(VulkanRenderTarget);

        VkSurfaceKHR GetSurface();
        [[nodiscard]] bool IsHeadless() const;
    private:
        TetherVulkan::Surface m_Surface;
        bool m_IsHeadless;
    };
}
