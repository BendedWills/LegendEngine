#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VertexBufferNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool VertexBufferNative::OnCreate(
	VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
	uint32_t* pIndices, uint64_t indexCount
)
{
    if (!pVertices || vertexCount == 0 || !pIndices || indexCount == 0)
        return false;
    
	// Create vertex buffer
	uint64_t verticesSize = sizeof(pVertices[0]) * vertexCount;
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = verticesSize;
		createInfo.usage =
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(pVulkanRenderer->allocator, &createInfo,
			&allocInfo, &vertexBuffer, &allocation, nullptr) != VK_SUCCESS)
			return false;
	}

    // Create index buffer
	uint64_t indicesSize = sizeof(pIndices[0]) * indexCount;
    {
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = indicesSize;
		createInfo.usage =
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(pVulkanRenderer->allocator, &createInfo,
			&allocInfo, &indexBuffer, &indexAlloc, nullptr) != VK_SUCCESS)
			return false;
    }

    if (!UploadData(pVertices, verticesSize, vertexBuffer))
        return false;
	if (!UploadData(pIndices, indicesSize, indexBuffer))
		return false;

    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(),"Vulkan::VertexBuffer", 
        "Initialized vertex buffer", 
        LogType::DEBUG
    );

    return true;
}

void VertexBufferNative::OnDispose()
{
    pVulkanRenderer->device.WaitIdle();

    vmaDestroyBuffer(pVulkanRenderer->allocator, vertexBuffer, allocation);
    vmaDestroyBuffer(pVulkanRenderer->allocator, indexBuffer, indexAlloc);
}

bool VertexBufferNative::UploadData(void* data, uint64_t dataBytes, VkBuffer buffer)
{
    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(), "Vulkan::VertexBuffer",
        "Staging vertex data...",
        LogType::DEBUG
    );

    VkBuffer stagingBuffer;
    VmaAllocation stagingAlloc;
    VmaAllocationInfo stagingAllocInfo;
	if (!pVulkanRenderer->CreateStagingBuffer(&stagingBuffer, &stagingAlloc,
		&stagingAllocInfo, dataBytes))
		return false;

    // Map the data to the staging buffer
    memcpy(stagingAllocInfo.pMappedData, data, dataBytes);

    // Create a command buffer and copy the data
    // Eventually, maybe use a queue specifically for transferring.
	VkCommandBuffer commandBuffer;
	if (!pVulkanRenderer->BeginSingleUseCommandBuffer(&commandBuffer))
	{
		vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer, stagingAlloc);
		return false;
	}
    {
		VkBufferCopy bufferCopy{};
		bufferCopy.size = dataBytes;

		vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &bufferCopy);
    }
	if (!pVulkanRenderer->EndSingleUseCommandBuffer(commandBuffer))
	{
		vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer, stagingAlloc);
		return false;
	}

    // Destroy the staging buffer
    vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer, stagingAlloc);

    return true;
}

#endif // VULKAN_API