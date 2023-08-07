/*
 * The shader program class wraps the pipeline, pipeline layout, and descriptor set
 * layout.
 */

#ifndef _LEGENDENGINE_VULKAN_PIPELINE_HPP
#define _LEGENDENGINE_VULKAN_PIPELINE_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/Common/PipelineInfo.hpp>

#include <vector>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer;
	class Pipeline : public IDisposable
	{
	public:
		Pipeline() {}
		LEGENDENGINE_NO_COPY(Pipeline);

		/**
		 * @brief Initializes the ShaderProgram.
		 *
		 * @param pRenderer A pointer to the renderer.
		 * @param pPipelineInfo A pointer to the PipelineInfo structure.
		 * 
		 * @returns True if initialization was successful; otherwise, false.
		 */
		bool Init(
			VulkanRenderer* pRenderer,
			PipelineInfo* pPipelineInfo
		);

		VkPipeline GetPipeline();
		VkPipelineLayout GetPipelineLayout();
	private:
		// Initializes pipeline and pipelineLayout.
		bool InitPipeline(
			PipelineInfo* pPipelineInfo
		);

		void OnDispose();
		
		VulkanRenderer* pRenderer = nullptr;
		
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_PIPELINE_HPP
