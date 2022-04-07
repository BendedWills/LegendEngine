/*
 * This file defines a structure that contains the details about a device's
 * support of a swapchain.
 */

#ifndef _LEGENDENGINE_VULKAN_PIPELINEINFO_HPP
#define _LEGENDENGINE_VULKAN_PIPELINEINFO_HPP

#include <LegendEngine/Common/Defs.hpp>

#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>

#include <vulkan/vulkan.h>
#include <iostream>

namespace LegendEngine::Vulkan
{
    struct PipelineInfo
	{
		uint64_t stageCount;
		VkPipelineShaderStageCreateInfo* pStages;
		uint64_t setCount;
		VkDescriptorSetLayout* pSetLayouts;
		uint64_t dynamicStateCount;
		VkDynamicState* pDynamicStates;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_PIPELINEINFO_HPP