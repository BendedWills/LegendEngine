#pragma once

#include <LegendEngine/Common/Assert.hpp>
#include <vulkan/vulkan.h>

#ifdef NDEBUG
#define LE_CHECK_VK(func) func
#else
#define LE_CHECK_VK(func) LE_ASSERT(func == VK_SUCCESS, "Vulkan function failed")
#define LE_CHECK_VK_MSG(func, ...) LE_ASSERT(func == VK_SUCCESS, __VA_ARGS__)
#endif