#pragma once
#include <LegendEngine/Graphics/ShaderManager.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class VulkanShaderManager final : public ShaderManager
    {
    public:
        VulkanShaderManager(TetherVulkan::GraphicsContext& graphicsContext,
            const std::span<VkDescriptorSetLayout>& setLayouts);
    private:
        TetherVulkan::GraphicsContext& m_GraphicsContext;
        std::span<VkDescriptorSetLayout> m_SetLayouts;

        VulkanShader m_SolidShader;
        VulkanShader m_TexturedShader;
    };
}
