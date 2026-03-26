#include <../Vulkan/VkDefs.hpp>
#include <../Vulkan/VulkanTexture2DArray.hpp>
#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>

namespace le
{
	VulkanTexture2DArray::VulkanTexture2DArray(
		Tether::Rendering::Vulkan::GraphicsContext& context,
		const size_t width, const size_t height, const uint8_t channels,
		const std::span<TextureData*>& textureData, std::mutex& mutex)
		:
		Texture2DArray(width, height, channels),
		m_Context(context)
	{
		const VkFormat format = FindImageFormat(channels);
		CreateImage(format, textureData.size(), width, height);
		CreateImageView(format, textureData.size());
		CreateSampler();

		const size_t bytesPerPixel = width * height * channels;
		UploadData(width, height, bytesPerPixel, textureData, mutex);
	}

	VulkanTexture2DArray::~VulkanTexture2DArray()
	{
		vkDestroyImageView(m_Context.GetDevice(), m_ImageView, nullptr);
		vkDestroySampler(m_Context.GetDevice(), m_Sampler, nullptr);
		vmaDestroyImage(m_Context.GetAllocator(), m_Image, m_ImageAllocation);
	}

	VkDescriptorType VulkanTexture2DArray::GetDescriptorType()
	{
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}

	VkDescriptorImageInfo VulkanTexture2DArray::GetImageInfo(uint32_t setIndex)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_ImageView;
		imageInfo.sampler = m_Sampler;

		return imageInfo;
	}

	Texture::APIBridge* VulkanTexture2DArray::GetAPIBridge()
	{
		return this;
	}

	void VulkanTexture2DArray::CreateImage(const VkFormat format, const size_t arrayLayers,
		const size_t width, const size_t height)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = arrayLayers;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.format = format;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

		LE_CHECK_VK(vmaCreateImage(
			m_Context.GetAllocator(),
			&imageInfo,
			&allocInfo,
			&m_Image,
			&m_ImageAllocation,
			nullptr
		));
	}

	void VulkanTexture2DArray::CreateImageView(const VkFormat format,
		const size_t arrayLayers)
	{
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = m_Image;
		viewCreateInfo.format = format;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = arrayLayers;

		LE_CHECK_VK(vkCreateImageView(m_Context.GetDevice(), &viewCreateInfo, nullptr, &m_ImageView));
	}

	void VulkanTexture2DArray::CreateSampler()
	{
		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
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

		LE_CHECK_VK(vkCreateSampler(m_Context.GetDevice(), &createInfo, nullptr, &m_Sampler));
	}

	void VulkanTexture2DArray::UploadData(
		const uint32_t width, const uint32_t height, const size_t size,
		const std::span<TextureData*>& textureData, std::mutex& mutex) const
	{
		VmaAllocationInfo stagingInfo{};
		VmaAllocation allocation = nullptr;
		VkBuffer buffer = nullptr;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size * textureData.size();
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
			| VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		LE_CHECK_VK(vmaCreateBuffer(m_Context.GetAllocator(), &bufferInfo,
			&allocInfo, &buffer, &allocation, &stagingInfo));

		CopyImagesToBuffer(static_cast<uint8_t*>(stagingInfo.pMappedData), size,
			textureData);

		Tether::Rendering::Vulkan::SingleUseCommandBuffer cmdBuffer(m_Context);
		VkCommandBuffer c = cmdBuffer.Begin();
		{
			// Transition all layers to transfer dst at once
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.image = m_Image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.layerCount = textureData.size();

			vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0, 0,
				nullptr, 0,
				nullptr, 1, &barrier);

			std::vector<VkBufferImageCopy> copies(textureData.size());
			for (size_t i = 0; i < copies.size(); i++)
			{
				copies[i].bufferOffset = i * size;
				copies[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copies[i].imageSubresource.baseArrayLayer = i;
				copies[i].imageSubresource.layerCount = 1;
				copies[i].imageOffset = { 0, 0, 0 };
				copies[i].imageExtent = { width, height, 1 };
			}

			vkCmdCopyBufferToImage(c,
				buffer,
				m_Image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				copies.size(),
				copies.data());

			// Transition to shader read
			barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(c,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0, 0, nullptr, 0, nullptr, 1, &barrier);
		}
		cmdBuffer.End();

		{
			std::scoped_lock lock(mutex);
			cmdBuffer.Submit();
		}

		vmaDestroyBuffer(m_Context.GetAllocator(), buffer, allocation);
	}

	void VulkanTexture2DArray::CopyImagesToBuffer(uint8_t* data, const size_t size, const std::span<TextureData*>& textureData)
	{
		for (size_t i = 0; i < textureData.size(); i++)
		{
			const TextureData* texture = textureData[i];
			const size_t textureSize = texture->GetWidth() * texture->GetHeight()
				* texture->GetChannels();

			LE_ASSERT(textureSize == size, "All textures given to Texture2DArray must be the same size");

			memcpy(data + i * size, texture->GetData(), size);
		}
	}
}
