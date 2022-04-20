#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VertexBufferNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool VertexBufferNative::OnCreate(VertexTypes::Vertex3c* pVertices,
    uint64_t vertexCount)
{
    if (!pVertices || vertexCount == 0)
        return false;
    
    verticesSize = sizeof(pVertices[0]) * vertexCount;

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

    if (!UploadVertexData(pVertices))
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
}

bool VertexBufferNative::UploadVertexData(void* pVertices)
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
		&stagingAllocInfo, verticesSize))
		return false;

    // Map the data to the staging buffer
    memcpy(stagingAllocInfo.pMappedData, pVertices, verticesSize);

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
		bufferCopy.size = verticesSize;

		vkCmdCopyBuffer(commandBuffer, stagingBuffer, vertexBuffer, 1, &bufferCopy);
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