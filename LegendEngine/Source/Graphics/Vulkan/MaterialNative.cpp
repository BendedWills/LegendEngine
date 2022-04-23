#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/MaterialNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool MaterialNative::OnCreate()
{
	this->images = pVulkanRenderer->swapchain.GetImageCount();

	VkDescriptorPoolSize uniformsSize{};
	uniformsSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformsSize.descriptorCount = images;

	VkDescriptorPoolSize samplersSize{};
	samplersSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplersSize.descriptorCount = images;

	VkDescriptorPoolSize sizes[] =
	{
		uniformsSize,
		samplersSize
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = sizeof(sizes) / sizeof(sizes[0]);
	poolInfo.pPoolSizes = sizes;
	poolInfo.maxSets = poolInfo.poolSizeCount * images;

	if (vkCreateDescriptorPool(pVulkanRenderer->device.Get(), &poolInfo, nullptr, &pool)
		!= VK_SUCCESS)
		return false;

	if (!uniform.Init(pVulkanRenderer, sizeof(Resources::Material::MaterialUniforms),
		images))
	{
		vkDestroyDescriptorPool(pVulkanRenderer->device.Get(), pool, nullptr);
		return false;
	}

	// Allocate the descriptor sets for the uniform
	uniform.BindToSet(&pool, pVulkanRenderer->materialLayout);

	UpdateDescriptorSets();

	return true;
}

void MaterialNative::OnUpdate()
{
	UpdateDescriptorSets();

	for (uint32_t i = 0; i < images; i++)
		uniform.UpdateBuffer(
			pMaterial->GetUniforms(),
			sizeof(Resources::Material::MaterialUniforms),
			i
		);
}

void MaterialNative::OnDispose()
{
	pVulkanRenderer->device.WaitIdle();

	uniform.Dispose();
	vkDestroyDescriptorPool(pVulkanRenderer->device.Get(), pool, nullptr);
}

void MaterialNative::UpdateDescriptorSets()
{
	// Wait for all frames to finish rendering.
	// Command buffers cannot be reset during frame rendering.
	for (uint64_t i2 = 0; i2 < pVulkanRenderer->inFlightFences.size(); i2++)
		vkWaitForFences(
			pVulkanRenderer->device.Get(), 1, 
			&pVulkanRenderer->inFlightFences[i2], true,
			UINT64_MAX
		);

	for (uint64_t i = 0; i < images; i++)
	{
		uint64_t writeSize = 1;

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(Resources::Material::MaterialUniforms);
		uniform.GetBuffer(&bufferInfo.buffer, i);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		uniform.GetDescriptorSet(&descriptorWrite.dstSet, i);

		// Write texture if it exists
		VkWriteDescriptorSet imageWrite{};
		VkDescriptorImageInfo imageInfo{};
		if (pMaterial->GetTexture())
		{
			writeSize++;

			Texture2DNative* texNative =
				(Texture2DNative*)pMaterial->GetTexture()->GetNative();

			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.sampler = texNative->sampler;
			imageInfo.imageView = texNative->imageView;

			imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			imageWrite.dstBinding = 1;
			imageWrite.dstArrayElement = 0;
			imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			imageWrite.descriptorCount = 1;
			imageWrite.pImageInfo = &imageInfo;
			uniform.GetDescriptorSet(&imageWrite.dstSet, i);
		}

		VkWriteDescriptorSet descWrites[] =
		{
			// The guaranteed descriptor writes go first
			descriptorWrite,
			// Then the ones that might fail
			imageWrite
		};

		vkUpdateDescriptorSets(pVulkanRenderer->device.Get(), writeSize, descWrites, 0,
			nullptr);
	}
}

#endif // VULKAN_API