#include <../Vulkan/ShaderModule.hpp>
#include <../Vulkan/VulkanShader.hpp>

namespace le
{
    using namespace Tether::Rendering::Vulkan;

    VulkanShader::VulkanShader(GraphicsContext& context,
        const std::span<Stage> stages,
        const std::span<VkDescriptorSetLayout> sets,
        VkFormat depthFormat)
        :
        m_Context(context),
        m_Pipeline(CreatePipeline(stages, sets, depthFormat))
    {}

    VkPipeline VulkanShader::GetPipeline() const
    {
        return m_Pipeline.Get();
    }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const
    {
        return m_Pipeline.GetPipelineLayout();
    }

    VkCullModeFlags VulkanShader::GetVulkanCullMode() const
    {
        switch (m_CullMode)
        {
            case CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT;
            case CullMode::BACK: return VK_CULL_MODE_BACK_BIT;
            case CullMode::NONE: return VK_CULL_MODE_NONE;
        }

        return VK_CULL_MODE_NONE;
    }

    Pipeline VulkanShader::CreatePipeline(const std::span<Stage> stages,
                                          std::span<VkDescriptorSetLayout> sets, VkFormat depthFormat) const
    {
        std::vector<VkPipelineShaderStageCreateInfo> vkStages;
        std::vector<ShaderModule> shaderModules;
        vkStages.reserve(stages.size());
        shaderModules.reserve(stages.size());

        for (Stage stage : stages)
        {
            shaderModules.emplace_back(m_Context, stage);

            VkPipelineShaderStageCreateInfo vkStage{};
            vkStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vkStage.module = shaderModules.back().Get();
            vkStage.pName = "main";

            switch (stage.type)
            {
                case ShaderType::VERTEX: vkStage.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
                case ShaderType::FRAG:  vkStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                case ShaderType::COMPUTE: vkStage.stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
                case ShaderType::GEOM:  vkStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            }

            vkStages.push_back(vkStage);
        }

        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_CULL_MODE
        };

        Pipeline::Info pipelineInfo{};
        pipelineInfo.stageCount = vkStages.size();
        pipelineInfo.pStages = vkStages.data();
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.dynamicStateCount = std::size(dynamicStates);
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.setCount = sets.size();
        pipelineInfo.pSetLayouts = sets.data();
        pipelineInfo.depthFormat = depthFormat;

        return { m_Context, pipelineInfo };
    }
}
