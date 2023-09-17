#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/MaterialNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

namespace LegendEngine::Vulkan
{

	MaterialNative::MaterialNative(TetherVulkan::GraphicsContext& context,
		uint32_t images, VkDescriptorSetLayout layout,
		Resources::Material* pMaterial)
		:
		Resources::IMaterialNative(pMaterial),
		m_GraphicsContext(context),
		m_Device(context.GetDevice()),
		m_Layout(layout),
		images(images)
	{}

	bool MaterialNative::OnCreate()
	{
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

		if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &pool)
			!= VK_SUCCESS)
			return false;

		uniform.emplace(m_GraphicsContext,
			sizeof(Resources::Material::MaterialUniforms), images);

		// Allocate the descriptor sets for the uniform
		uniform->BindToSet(&pool, m_Layout);

		UpdateDescriptorSets();

		return true;
	}

	void MaterialNative::OnUpdate()
	{
		UpdateDescriptorSets();

		for (uint32_t i = 0; i < images; i++)
			uniform->UpdateBuffer(
				pMaterial->GetUniforms(),
				sizeof(Resources::Material::MaterialUniforms),
				i
			);
	}

	void MaterialNative::OnDispose()
	{
		vkDeviceWaitIdle(m_Device);

		vkDestroyDescriptorPool(m_Device, pool, nullptr);
	}

	void MaterialNative::UpdateDescriptorSets()
	{
		// Wait for all frames to finish rendering.
		// Command buffers cannot be reset during frame rendering.
		vkDeviceWaitIdle(m_Device);

		for (uint64_t i = 0; i < images; i++)
		{
			uint64_t writeSize = 1;

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Resources::Material::MaterialUniforms);
			bufferInfo.buffer = uniform->GetBuffer(i);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.dstSet = uniform->GetDescriptorSet(i);

			// Write texture if it exists
			VkWriteDescriptorSet imageWrite{};
			VkDescriptorImageInfo imageInfo{};
			Resources::Texture2D* pTexture = pMaterial->GetTexture();
			if (pTexture && pTexture->IsInitialized())
			{
				writeSize++;

				Texture2DNative* texNative = (Texture2DNative*)pTexture->GetNative();

				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.sampler = texNative->sampler;
				imageInfo.imageView = texNative->imageView;

				imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				imageWrite.dstBinding = 1;
				imageWrite.dstArrayElement = 0;
				imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				imageWrite.descriptorCount = 1;
				imageWrite.pImageInfo = &imageInfo;
				imageWrite.dstSet = uniform->GetDescriptorSet(i);
			}

			VkWriteDescriptorSet descWrites[] =
			{
				// The guaranteed descriptor writes go first
				descriptorWrite,
				// Then the ones that might fail
				imageWrite
			};

			vkUpdateDescriptorSets(m_Device, writeSize, descWrites, 0, nullptr);
		}
	}
}