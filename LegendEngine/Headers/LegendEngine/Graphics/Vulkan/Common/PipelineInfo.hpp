/*
 * This file defines a structure that contains the details about a device's
 * support of a swapchain.
 */

#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>

#include <vulkan/vulkan.h>
#include <iostream>

namespace LegendEngine::Vulkan
{
    struct PipelineInfo
	{
		uint32_t stageCount;
		VkPipelineShaderStageCreateInfo* pStages;
		uint32_t setCount;
		VkDescriptorSetLayout* pSetLayouts;
		uint32_t dynamicStateCount;
		VkDynamicState* pDynamicStates;
	};
}
