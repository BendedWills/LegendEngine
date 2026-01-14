#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/ShaderNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

namespace LegendEngine::Vulkan
{
	ShaderNative::ShaderNative(VulkanRenderer* pRenderer, Resources::Shader* pShader,
			std::span<Resources::ShaderStage> stages)
			:
			IShaderNative(pShader, stages),
			pVulkanRenderer(pRenderer)
	{
		std::vector<VkPipelineShaderStageCreateInfo> vkStages;
		std::vector<Scope<ShaderModule>> shaderModules;
		vkStages.reserve(stages.size());
		shaderModules.reserve(stages.size());

		for (Resources::ShaderStage stage : stages)
		{
			shaderModules.push_back(std::make_unique<ShaderModule>(
				pRenderer->m_GraphicsContext, stage));

			VkPipelineShaderStageCreateInfo vkStage{};
			vkStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vkStage.module = shaderModules.back()->Get();
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

		VkDescriptorSetLayout sets[] =
		{
			pRenderer->cameraLayout,
			pRenderer->materialLayout,
			pRenderer->objectLayout
		};

		PipelineInfo pipelineInfo{};
		pipelineInfo.stageCount = vkStages.size();
		pipelineInfo.pStages = vkStages.data();
		pipelineInfo.pDynamicStates = dynamicStates;
		pipelineInfo.dynamicStateCount = std::size(dynamicStates);
		pipelineInfo.pDynamicStates = dynamicStates;
		pipelineInfo.setCount = std::size(sets);
		pipelineInfo.pSetLayouts = sets;

		shaderProgram.emplace(
			pRenderer->m_GraphicsContext,
			pRenderer->m_Swapchain->GetExtent(),
			pRenderer->renderPass, pipelineInfo);

		LEGENDENGINE_OBJECT_LOG(
			pVulkanRenderer->GetApplication(), "Vulkan::Shader",
			"Initialized shader.",
			LogType::DEBUG
		);
	}

	Pipeline& ShaderNative::GetPipeline()
	{
		return shaderProgram.value();
	}

	void ShaderNative::OnDispose()
	{

	}
}