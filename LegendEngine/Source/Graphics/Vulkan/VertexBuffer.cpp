#include <LegendEngine/Graphics/Vulkan/VertexBuffer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool VertexBuffer::OnBufferCreate(VertexTypes::Vertex2* pVertices, 
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

void VertexBuffer::OnBufferDispose()
{
    vmaDestroyBuffer(pVulkanRenderer->allocator, vertexBuffer, allocation);
}

bool VertexBuffer::UploadVertexData(void* pVertices)
{
    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(), "Vulkan::VertexBuffer",
        "Staging vertex data...",
        LogType::DEBUG
    );

    VkBuffer stagingBuffer;
    VmaAllocation stagingAlloc;
    VmaAllocationInfo stagingAllocInfo;

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = verticesSize;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    // Create the staging buffer
    if (vmaCreateBuffer(pVulkanRenderer->allocator, &createInfo,
        &allocInfo, &stagingBuffer, &stagingAlloc, &stagingAllocInfo) != VK_SUCCESS)
        return false;

    // Map the data to the staging buffer
    memcpy(stagingAllocInfo.pMappedData, pVertices, verticesSize);

    // Create a command buffer and copy the data
    // Eventually, maybe use a queue specifically for transfering.
    {
        VkCommandBuffer stagingCmdBuffer;

        VkCommandBufferAllocateInfo cmdAllocInfo{};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandPool = pVulkanRenderer->commandPool;
        cmdAllocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(pVulkanRenderer->device.Get(), &cmdAllocInfo,
            &stagingCmdBuffer) != VK_SUCCESS)
        {
            vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer, 
                stagingAlloc);
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // +1 good practice points
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(stagingCmdBuffer, &beginInfo);
        {
            VkBufferCopy bufferCopy{};
            bufferCopy.size = verticesSize;
            vkCmdCopyBuffer(stagingCmdBuffer, stagingBuffer, vertexBuffer, 1,
                &bufferCopy);
        }
        vkEndCommandBuffer(stagingCmdBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &stagingCmdBuffer;
        
        if (vkQueueSubmit(pVulkanRenderer->graphicsQueue, 1, &submitInfo,
            VK_NULL_HANDLE) != VK_SUCCESS)
        {
            vkFreeCommandBuffers(pVulkanRenderer->device.Get(),
                pVulkanRenderer->commandPool, 1, &stagingCmdBuffer);
            vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer,
                stagingAlloc);
            return false;
        }

        // Wait for the data to finish uploading
        vkQueueWaitIdle(pVulkanRenderer->graphicsQueue);

        // Free the command buffer
        vkFreeCommandBuffers(pVulkanRenderer->device.Get(),
            pVulkanRenderer->commandPool, 1, &stagingCmdBuffer);
    }

    // Destroy the staging buffer
    vmaDestroyBuffer(pVulkanRenderer->allocator, stagingBuffer, stagingAlloc);

    return true;
}
