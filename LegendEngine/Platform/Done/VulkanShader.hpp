#pragma once

#include "API/Pipeline.hpp"

#include <vulkan/vulkan.h>

namespace le
{
    class VulkanShader
    {
    public:
        VulkanShader(Tether::Rendering::Vulkan::GraphicsContext& context,
            std::span<Stage> stages, std::span<VkDescriptorSetLayout> sets,
            VkFormat depthFormat);

        VkPipeline GetPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;
        VkCullModeFlags GetVulkanCullMode() const;
    private:
        Pipeline CreatePipeline(std::span<Stage> stages,
            std::span<VkDescriptorSetLayout> sets, VkFormat depthFormat) const;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        Pipeline m_Pipeline;
    };
}
