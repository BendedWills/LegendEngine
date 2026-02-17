#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanShader.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Tether::Rendering::Vulkan;

    VulkanShader::VulkanShader(GraphicsContext& context,
        const std::span<Stage> stages,
        const std::span<VkDescriptorSetLayout> sets)
        :
        m_Context(context),
        m_Pipeline(CreatePipeline(stages, sets))
    {}

    VkPipeline VulkanShader::GetPipeline() const
    {
        return m_Pipeline.Get();
    }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const
    {
        return m_Pipeline.GetPipelineLayout();
    }

    Pipeline VulkanShader::CreatePipeline(const std::span<Stage> stages,
        std::span<VkDescriptorSetLayout> sets) const
    {
        std::vector<VkPipelineShaderStageCreateInfo> vkStages;
        std::vector<ShaderModule> shaderModules;
        vkStages.reserve(stages.size());
        shaderModules.reserve(stages.size());

        for (Stage stage : stages)
        {
            shaderModules.push_back(ShaderModule(m_Context, stage));

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
            VK_DYNAMIC_STATE_SCISSOR
        };

        Pipeline::Info pipelineInfo{};
        pipelineInfo.stageCount = vkStages.size();
        pipelineInfo.pStages = vkStages.data();
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.dynamicStateCount = std::size(dynamicStates);
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.setCount = sets.size();
        pipelineInfo.pSetLayouts = sets.data();

        return Pipeline(m_Context, pipelineInfo);
    }
}
