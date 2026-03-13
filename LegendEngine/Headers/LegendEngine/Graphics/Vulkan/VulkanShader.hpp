#pragma once

#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>

#include <vulkan/vulkan.h>

namespace le
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(Tether::Rendering::Vulkan::GraphicsContext& context,
            std::span<Stage> stages, std::span<VkDescriptorSetLayout> sets,
            VkFormat depthFormat);

        VkPipeline GetPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;
    private:
        Pipeline CreatePipeline(std::span<Stage> stages,
            std::span<VkDescriptorSetLayout> sets, VkFormat depthFormat) const;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        Pipeline m_Pipeline;
    };
}
