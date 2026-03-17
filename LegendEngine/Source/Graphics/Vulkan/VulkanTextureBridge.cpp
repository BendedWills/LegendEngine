#include <LegendEngine/Graphics/Vulkan/VulkanTextureBridge.hpp>

namespace le
{
	VkFormat VulkanTextureBridge::FindImageFormat(uint32_t channels)
	{
		VkFormat format;
		switch (channels)
		{
			case 1: format = VK_FORMAT_R8_SRGB; break;
			case 3: format = VK_FORMAT_R8G8B8_SRGB; break;
			case 4: format = VK_FORMAT_R8G8B8A8_SRGB; break;

			default: throw std::runtime_error("Unsupported image format");
		}

		return format;
	}
}
