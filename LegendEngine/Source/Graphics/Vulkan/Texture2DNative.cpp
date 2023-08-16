#include <LegendEngine/Graphics/Vulkan/Texture2DNative.hpp>

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

namespace LegendEngine::Vulkan
{
	Texture2DNative::Texture2DNative(TetherVulkan::GraphicsContext& context, 
		Resources::Texture2D* pTexture)
		:
		Resources::ITexture2DNative(pTexture),
		m_Context(context),
		m_Device(context.GetDevice())
	{}

	bool Texture2DNative::OnCreate(uint64_t width, uint64_t height, uint32_t channels,
		uint8_t* data)
	{
		if (width == 0 || height == 0 || !data)
			return false;

		this->width = width;
		this->height = height;
		this->channels = channels;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		switch (channels)
		{
		case 1: imageFormat = VK_FORMAT_R8_SRGB; break;
		case 3: imageFormat = VK_FORMAT_R8G8B8_SRGB; break;
		case 4: imageFormat = VK_FORMAT_R8G8B8A8_SRGB; break;

		default: return false;
		}

		imageInfo.format = imageFormat;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(m_Context.GetAllocator(), &imageInfo, &allocInfo, &image,
			&imageAllocation, nullptr) != VK_SUCCESS)
			return false;


		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = image;
		viewCreateInfo.format = imageFormat;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_Device, &viewCreateInfo, nullptr, &imageView)
			!= VK_SUCCESS)
		{
			vkDeviceWaitIdle(m_Device);
			vmaDestroyImage(m_Context.GetAllocator(), image, imageAllocation);
			throw std::runtime_error("Failed to create image view");
		}

		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.anisotropyEnable = false;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_Context.GetPhysicalDevice(), &properties);

		createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = false;
		createInfo.compareEnable = false;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Device, &createInfo, nullptr, &sampler)
			!= VK_SUCCESS)
		{
			vkDeviceWaitIdle(m_Device);
			vmaDestroyImage(m_Context.GetAllocator(), image, imageAllocation);
			vkDestroyImageView(m_Device, imageView, nullptr);
			return false;
		}

		StageImageData(data);

		// TODO: Log Initialized Texture2D

		return true;
	}

	void Texture2DNative::OnDispose()
	{
		// TODO: Log Disposed Texture2D

		vkDeviceWaitIdle(m_Device);

		vkDestroyImageView(m_Device, imageView, nullptr);
		vkDestroySampler(m_Device, sampler, nullptr);
		vmaDestroyImage(m_Context.GetAllocator(), image, imageAllocation);
	}

	void Texture2DNative::StageImageData(uint8_t* data)
	{
		// TODO: Log Staging vertex data...

		m_Stager.emplace(m_Context, image, width, height, channels, data, imageFormat);
		m_Stager->Upload();
	}
}
