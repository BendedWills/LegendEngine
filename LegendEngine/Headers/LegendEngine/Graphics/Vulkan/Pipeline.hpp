/*
 * The shader program class wraps the pipeline, pipeline layout, and descriptor set
 * layout.
 */

#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/Common/PipelineInfo.hpp>

#include <vector>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer;
	class Pipeline
	{
	public:
		Pipeline(
			TetherVulkan::GraphicsContext& context, 
			VkExtent2D swapchainExtent,
			VkRenderPass renderPass,
			const PipelineInfo& pipelineInfo);
		~Pipeline();
		LEGENDENGINE_NO_COPY(Pipeline);

		VkPipeline GetPipeline();
		VkPipelineLayout GetPipelineLayout();
	private:
		VkDevice m_Device = nullptr;
		
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
}
