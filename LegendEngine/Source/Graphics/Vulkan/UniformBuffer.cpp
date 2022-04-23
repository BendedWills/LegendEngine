#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <Tether/Native.hpp>

using namespace LegendEngine::Vulkan;

bool UniformBuffer::Init(VulkanRenderer* pRenderer, uint64_t size, uint32_t images)
{
    if (initialized)
        return false;

    this->pRenderer = pRenderer;
	this->images = images;
	this->size = size;
    
    // It isn't needed for the uniform buffers to have buffers for each image but
    // this is specific to this scenario since the command buffers aren't being 
    // refreshed each frame.
    descriptorSets.resize(images);
    allocations.resize(images);
	allocInfos.resize(images);
    uniformBuffers.resize(images);

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    for (uint64_t i = 0; i < images; i++)
		if (vmaCreateBuffer(pRenderer->allocator, &createInfo,
			&allocInfo, &uniformBuffers[i], &allocations[i], &allocInfos[i]) != VK_SUCCESS)
			return false;

    initialized = true;
    return true;
}

bool UniformBuffer::BindToSet(UniformManager* manager, VkDescriptorSetLayout layout)
{
	return BindToSet(manager->GetPool(), layout);
}

bool UniformBuffer::BindToSet(VkDescriptorPool* pPool, VkDescriptorSetLayout layout)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	FreeSet();

	this->pDescriptorPool = pPool;

	std::vector<VkDescriptorSetLayout> layouts(images, layout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = *pDescriptorPool;
	allocInfo.descriptorSetCount = images;
	allocInfo.pSetLayouts = layouts.data();

	if (vkAllocateDescriptorSets(pRenderer->device.Get(), &allocInfo,
		descriptorSets.data()) != VK_SUCCESS)
		return false;

	boundToSet = true;
	return true;
}

bool UniformBuffer::Bind(uint32_t binding)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	if (!boundToSet)
		return false;

	for (uint64_t i = 0; i < images; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = size;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(pRenderer->device.Get(), 1, &descriptorWrite, 0,
			nullptr);
	}

	return true;
}

bool UniformBuffer::UpdateBuffer(void* newData, uint64_t size, 
	uint64_t currentImage)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	if (size > this->size || size == 0 || currentImage >= images)
		return false;
    
    memcpy(allocInfos[currentImage].pMappedData, newData, size);
    
    return true;
}

bool UniformBuffer::GetBuffer(VkBuffer* pBuffer, uint64_t index)
{
	if (index >= uniformBuffers.size())
		return false;

	*pBuffer = uniformBuffers[index];

	return true;
}

bool UniformBuffer::GetDescriptorSet(VkDescriptorSet* pSet, uint64_t index)
{
	if (index >= descriptorSets.size())
		return false;

	*pSet = descriptorSets[index];

	return true;
}

void UniformBuffer::FreeSet()
{
	if (!boundToSet)
		return;

	vkFreeDescriptorSets(pRenderer->device.Get(), *pDescriptorPool,
		static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());

	boundToSet = false;
}

void UniformBuffer::OnDispose()
{
	FreeSet();

	for (uint64_t i = 0; i < images; i++)
        vmaDestroyBuffer(pRenderer->allocator, uniformBuffers[i], allocations[i]);

    descriptorSets.clear();
    allocations.clear();
    allocInfos.clear();
    uniformBuffers.clear();
}

#endif // VULKAN_API