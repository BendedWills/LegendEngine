#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <Tether/Common/Storage.hpp>

using namespace LegendEngine::Vulkan;

bool UniformBuffer::Init(VulkanRenderer* pRenderer, uint64_t size,
	uint64_t binding, VkShaderStageFlags shaderStages, uint64_t images)
{
    if (initialized)
        return false;

    this->pRenderer = pRenderer;
    
    this->binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    this->binding.descriptorCount = 1;
    this->binding.binding = binding;
    this->binding.stageFlags = shaderStages;
    this->size = size;
    this->images = images;

    // It isn't needed for the uniform buffers to have buffers for each image but
    // this is specific to this scenario since the command buffers aren't being 
    // refreshed each frame.
    descriptorSets.resize(images);
    allocations.resize(images);
    uniformBuffers.resize(images);

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    for (uint64_t i = 0; i < images; i++)
		if (vmaCreateBuffer(pRenderer->allocator, &createInfo,
			&allocInfo, &uniformBuffers[i], &allocations[i], nullptr) != VK_SUCCESS)
			return false;

    initialized = true;
    return true;
}

bool UniformBuffer::UpdateBuffer(void* newData, uint64_t currentImage)
{
    void* data;
    vmaMapMemory(pRenderer->allocator, allocations[currentImage], &data);
        memcpy(data, newData, size);
    vmaUnmapMemory(pRenderer->allocator, allocations[currentImage]);

    return true;
}

VkDescriptorSetLayoutBinding UniformBuffer::GetUboLayoutBinding()
{
    return binding;
}

std::vector<VkDescriptorSet>& UniformBuffer::GetDescriptorSets()
{
	return descriptorSets;
}

bool UniformBuffer::InitByPipeline(VkDescriptorSetLayout& layout, 
    VkDescriptorPool* pDescPool)
{
    if (initByPipeline)
        return false;

    this->pDescriptorPool = pDescPool;

    std::vector<VkDescriptorSetLayout> layouts(images, layout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = *pDescriptorPool;
	allocInfo.descriptorSetCount = images;
	allocInfo.pSetLayouts = layouts.data();

    VkResult result = vkAllocateDescriptorSets(pRenderer->device.Get(), &allocInfo,
        descriptorSets.data());
	if (result != VK_SUCCESS)
	{
		vkDestroyDescriptorSetLayout(pRenderer->device.Get(),
			layout, nullptr);
		return false;
	}

	for (uint64_t i = 0; i < images; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = size;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = binding.binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(pRenderer->device.Get(), 1, &descriptorWrite, 0, 
            nullptr);
	}

    initByPipeline = true;
    return true;
}

void UniformBuffer::OnDispose()
{
    if (initByPipeline)
		vkFreeDescriptorSets(pRenderer->device.Get(), *pDescriptorPool,
			descriptorSets.size(), descriptorSets.data());

	for (uint64_t i = 0; i < images; i++)
        vmaDestroyBuffer(pRenderer->allocator, uniformBuffers[i], allocations[i]);

    descriptorSets.clear();
    allocations.clear();
    uniformBuffers.clear();

    initByPipeline = false;
}

#endif // VULKAN_API