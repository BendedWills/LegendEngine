/*
 * This file defines a structure that contains the details about a device's
 * support of a swapchain.
 */

#ifndef _LEGENDENGINE_VULKAN_SWAPCHAINDETAILS_HPP
#define _LEGENDENGINE_VULKAN_SWAPCHAINDETAILS_HPP
#ifdef VULKAN_API

#include <vulkan/vulkan.h>
#include <vector>

namespace LegendEngine::Vulkan
{
    struct SwapchainDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SWAPCHAINDETAILS_HPP