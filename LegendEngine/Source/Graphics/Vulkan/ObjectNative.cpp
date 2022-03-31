#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/ObjectNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;
using namespace LegendEngine::Objects;

ObjectNative::ObjectNative(VulkanRenderer* pRenderer, Object* pObject)
	:
	IObjectNative(pObject)
{
	this->pRenderer = pRenderer;
	this->images = pRenderer->swapchain.GetImageCount();

	// It isn't needed for the uniform buffers to have buffers for each image but
	// this is specific to this scenario since the command buffers aren't being 
	// refreshed each frame.
	descriptorSets.resize(images);
	allocations.resize(images);
	uniformBuffers.resize(images);

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = sizeof(IObjectNative::ObjectUniforms);
	createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	for (uint64_t i = 0; i < images; i++)
		if (vmaCreateBuffer(pRenderer->allocator, &createInfo,
			&allocInfo, &uniformBuffers[i], &allocations[i], nullptr) != VK_SUCCESS)
			return;

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = images;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = poolSize.descriptorCount;

	if (vkCreateDescriptorPool(pRenderer->device.Get(), &poolInfo, nullptr,
		&descriptorPool) != VK_SUCCESS)
		return;

	std::vector<VkDescriptorSetLayout> layouts(images, pRenderer->objectLayout);

	VkDescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = descriptorPool;
	setAllocInfo.descriptorSetCount = images;
	setAllocInfo.pSetLayouts = layouts.data();

	if (vkAllocateDescriptorSets(pRenderer->device.Get(), &setAllocInfo,
		descriptorSets.data()) != VK_SUCCESS)
		return;

	for (uint64_t i = 0; i < images; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = createInfo.size;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(pRenderer->device.Get(), 1, &descriptorWrite, 0,
			nullptr);
	}
}

void ObjectNative::SetCurrentImage(uint64_t imageIndex)
{
	this->imageIndex = imageIndex;
}

void ObjectNative::OnUniformsUpdate()
{
	IObjectNative::ObjectUniforms ubo;
	ubo.transform = pObject->GetTransformationMatrix();

	void* data;
	vmaMapMemory(pRenderer->allocator, allocations[imageIndex], &data);
		memcpy(data, &ubo, sizeof(ubo));
	vmaUnmapMemory(pRenderer->allocator, allocations[imageIndex]);
}

VkDescriptorSet* ObjectNative::GetDescriptorSets()
{
	return descriptorSets.data();
}

void ObjectNative::OnDispose()
{
	pRenderer->device.WaitIdle();

	vkFreeDescriptorSets(pRenderer->device.Get(), descriptorPool,
		descriptorSets.size(), descriptorSets.data());

	vkDestroyDescriptorPool(pRenderer->device.Get(), descriptorPool, nullptr);

	for (uint64_t i = 0; i < images; i++)
		vmaDestroyBuffer(pRenderer->allocator, uniformBuffers[i], allocations[i]);

	descriptorSets.clear();
	allocations.clear();
	uniformBuffers.clear();
}

#endif // VULKAN_API