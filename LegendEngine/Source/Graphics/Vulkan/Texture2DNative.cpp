#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/Texture2DNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool Texture2DNative::OnCreate(uint64_t width, uint64_t height, uint32_t channels,
    uint8_t* data)
{
    if (width == 0 || height == 0 || !data)
        return false;

    this->width = width;
    this->height = height;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8_SRGB;
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

    if (vmaCreateImage(pVulkanRenderer->allocator, &imageInfo, &allocInfo, &image,
        &imageAllocation, nullptr) != VK_SUCCESS)
        return false;

    if (!pVulkanRenderer->CreateImageView(&imageView, image, imageFormat, 
        VK_IMAGE_VIEW_TYPE_2D))
    {
        pVulkanRenderer->device.WaitIdle();
        vmaDestroyImage(pVulkanRenderer->allocator, image, imageAllocation);
        return false;
    }

    StageImageData(data);
    
    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(),"Vulkan::Texture2DNative", 
        "Initialized Texture2D", 
        LogType::DEBUG
    );

    return true;
}

void Texture2DNative::OnDispose()
{
	LEGENDENGINE_OBJECT_LOG(
		pVulkanRenderer->GetApplication(), "Vulkan::Texture2DNative",
		"Disposed Texture2D",
		LogType::DEBUG
	);

    pVulkanRenderer->device.WaitIdle();

    vkDestroyImageView(pVulkanRenderer->device.Get(), imageView, nullptr);
    vmaDestroyImage(pVulkanRenderer->allocator, image, imageAllocation);
}

bool Texture2DNative::StageImageData(uint8_t* data)
{
	LEGENDENGINE_OBJECT_LOG(
		pVulkanRenderer->GetApplication(), "Vulkan::VertexBuffer",
		"Staging vertex data...",
		LogType::DEBUG
	);

    uint64_t bufferSize = width * height * channels;

    if (!pVulkanRenderer->CreateStagingBuffer(&stagingBuffer, &stagingAlloc,
        &stagingAllocInfo, bufferSize))
        return false;

	// Map the data to the staging buffer
	memcpy(stagingAllocInfo.pMappedData, data, bufferSize);

    pVulkanRenderer->ChangeImageLayout(image, imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    pVulkanRenderer->CopyBufferToImage(stagingBuffer, image, width, height);

	pVulkanRenderer->ChangeImageLayout(
        image, imageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

	// Destroy the staging buffer
	vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer, stagingAlloc);

    return true;
}

#endif // VULKAN_API