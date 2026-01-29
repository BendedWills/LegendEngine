#pragma once

#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Graphics::Vulkan
{
    class VulkanShader : public Resources::Shader
    {
    public:
        VulkanShader(Tether::Rendering::Vulkan::GraphicsContext& context,
            std::span<Stage> stages, std::span<VkDescriptorSetLayout> sets,
            VkRenderPass renderPass);

        VkPipeline GetPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;
    private:
        Pipeline CreatePipeline(std::span<Stage> stages,
            std::span<VkDescriptorSetLayout> sets, VkRenderPass renderPass) const;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        Pipeline m_Pipeline;
    };
}
