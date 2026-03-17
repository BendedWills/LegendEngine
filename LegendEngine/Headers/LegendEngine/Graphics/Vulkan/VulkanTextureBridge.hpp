#pragma once
#include <LegendEngine/Resources/Texture.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSetWritable.hpp>

namespace le
{
	class VulkanTextureBridge : public Texture::APIBridge, public Tether::Rendering::Vulkan::DescriptorSetWritable
	{
	public:
		static VkFormat FindImageFormat(uint32_t channels);
	};
}
