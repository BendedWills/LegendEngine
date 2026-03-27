#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <Tether/Window.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

#include <vulkan/vulkan.h>

namespace le
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanRenderTarget final : public RenderTarget
    {
    public:
        explicit VulkanRenderTarget(TetherVulkan::GraphicsContext& context); // Headless
        VulkanRenderTarget(TetherVulkan::GraphicsContext& context, Tether::Window& window);
        LE_NO_COPY(VulkanRenderTarget);

        VkSurfaceKHR GetSurface();
        [[nodiscard]] bool IsHeadless() const;
    private:
        TetherVulkan::Surface m_Surface;
        bool m_IsHeadless;
    };
}
