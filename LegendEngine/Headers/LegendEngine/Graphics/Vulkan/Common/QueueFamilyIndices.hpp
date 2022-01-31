#ifndef _LEGENDENGINE_VULKAN_QUEUEFAMILYINDICES_HPP
#define _LEGENDENGINE_VULKAN_QUEUEFAMILYINDICES_HPP
#ifdef VULKAN_API

#include <iostream>

namespace LegendEngine::Vulkan
{
    struct QueueFamilyIndices
	{
		bool hasGraphicsFamily = false;
		bool hasPresentFamily = false;
		uint32_t graphicsFamilyIndex = 0;
		uint32_t presentFamilyIndex = 0;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_QUEUEFAMILYINDICES_HPP