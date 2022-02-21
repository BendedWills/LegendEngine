#include <LegendEngine/Graphics/Vulkan/VertexBuffer.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool VertexBuffer::OnBufferCreate(VertexTypes::Vertex2* pVertices, 
    uint64_t vertexCount)
{
    if (!VerifyRenderer())
        return false;
    
    uint64_t verticesSize = sizeof(pVertices[0]) * vertexCount;

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = verticesSize;
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(pVulkanRenderer->device.Get(), &createInfo, nullptr, 
        &vertexBuffer) != VK_SUCCESS)
        return false;
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(pVulkanRenderer->device.Get(), 
        vertexBuffer, &memoryRequirements);

    uint32_t memoryType;
    if (!FindMemoryType(memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &memoryType))
    {
        std::cout << "Failed to find suitable memory type!" << std::endl;
        return false;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = memoryType;
    
    if (vkAllocateMemory(pVulkanRenderer->device.Get(), &allocInfo, nullptr, 
        &vertexBufferMemory) != VK_SUCCESS)
    {
        std::cout << "Failed to allocate vertex buffer memory!" << std::endl;
        return false;
    }

    vkBindBufferMemory(pVulkanRenderer->device.Get(), vertexBuffer, 
        vertexBufferMemory, 0);

    void* data;
    vkMapMemory(pVulkanRenderer->device.Get(), vertexBufferMemory, 0, 
        createInfo.size, 0, &data);
        memcpy(data, pVertices, verticesSize);
    vkUnmapMemory(pVulkanRenderer->device.Get(), vertexBufferMemory);

    pVulkanRenderer->vertexBuffers.push_back(this);

    return true;
}

void VertexBuffer::OnBufferDispose()
{
    Tether::VectorUtils::EraseAll(pVulkanRenderer->vertexBuffers, this);
}

bool VertexBuffer::VerifyRenderer()
{
    return pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
}

bool VertexBuffer::FindMemoryType(uint32_t typeFilter, 
    VkMemoryPropertyFlags properties, uint32_t* pMemoryType)
{
    if (!pMemoryType)
        return false;
    
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(pVulkanRenderer->physicalDevice, 
        &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        if (typeFilter & (1 << i) 
            && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            *pMemoryType = i;
            return true;
        }
    
    return false;
}