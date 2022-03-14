/*
 * This file defines a structure that contains the details about a device's
 * support of a swapchain.
 */

#ifndef _LEGENDENGINE_VULKAN_PIPELINEINFO_HPP
#define _LEGENDENGINE_VULKAN_PIPELINEINFO_HPP
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>

#include <vulkan/vulkan.h>
#include <iostream>

namespace LegendEngine::Vulkan
{
    struct PipelineInfo
	{
		VkPipelineShaderStageCreateInfo* pStages;
		uint64_t stageCount;
		UniformBuffer** ppUniforms;
		uint64_t uniformCount;
		VkDescriptorSetLayoutCreateInfo* setLayouts;
		uint64_t setCount;
		VkDynamicState* pDynamicStates;
		uint64_t dynamicStateCount;
		// The number of swapchain images. Only necessary if uniforms are present.
		uint64_t images;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_PIPELINEINFO_HPP