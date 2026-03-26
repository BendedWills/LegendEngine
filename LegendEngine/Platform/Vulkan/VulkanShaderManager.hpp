#pragma once
#include <LE/Graphics/ShaderManager.hpp>
#include <../VulkanShader.hpp>

namespace le
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanShaderManager final : public ShaderManager
    {
    public:
        VulkanShaderManager(TetherVulkan::GraphicsContext& graphicsContext,
            const std::span<VkDescriptorSetLayout>& setLayouts,
            VkFormat depthFormat);
    private:
        TetherVulkan::GraphicsContext& m_GraphicsContext;
        std::span<VkDescriptorSetLayout> m_SetLayouts;

        VulkanShader m_SolidShader;
        VulkanShader m_TexturedShader;
    };
}
