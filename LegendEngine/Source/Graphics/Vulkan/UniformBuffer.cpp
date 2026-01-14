#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>
#include <LegendEngine/Objects/Camera.hpp>

#include <stdexcept>

namespace LegendEngine::Vulkan
{
	UniformBuffer::UniformBuffer(TetherVulkan::GraphicsContext& context, 
		size_t size, uint32_t images)
		:
		m_GraphicsContext(context),
		m_Images(images),
		m_Size(size)
	{
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
		{
			if (vmaCreateBuffer(m_GraphicsContext.GetAllocator(), &createInfo,
				&allocInfo, &uniformBuffers[i], &allocations[i], &allocInfos[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create uniform buffer");

#if !defined(NDEBUG)
			vmaSetAllocationName(m_GraphicsContext.GetAllocator(), allocations[i],
				"UniformBuffer allocation");
#endif
		}
	}

	UniformBuffer::~UniformBuffer()
	{
#if !defined(NDEBUG)
		if (this->pUniformManager)
			this->pUniformManager->UnregisterUniformBuffer(this);
#endif

		vkDeviceWaitIdle(m_GraphicsContext.GetDevice());

		FreeSet();

		for (uint64_t i = 0; i < m_Images; i++)
			vmaDestroyBuffer(m_GraphicsContext.GetAllocator(), uniformBuffers[i], allocations[i]);
	}

	void UniformBuffer::BindToSet(UniformManager* manager, VkDescriptorSetLayout layout)
	{
#if !defined(NDEBUG)
		if (this->pUniformManager)
			this->pUniformManager->UnregisterUniformBuffer(this);

		this->pUniformManager = manager;
		this->pUniformManager->RegisterUniformBuffer(this);
#endif

		BindToSet(manager->GetPool(), layout);
	}

	void UniformBuffer::BindToSet(VkDescriptorPool* pPool, VkDescriptorSetLayout layout)
	{
		FreeSet();

		this->pDescriptorPool = pPool;

		std::vector<VkDescriptorSetLayout> layouts(m_Images, layout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = *pDescriptorPool;
		allocInfo.descriptorSetCount = m_Images;
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(m_GraphicsContext.GetDevice(), &allocInfo,
			descriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate descriptor sets");

		boundToSet = true;
	}

	void UniformBuffer::Bind(uint32_t binding)
	{
		if (!boundToSet)
			throw std::runtime_error("Uniform buffer not bound to set");

		for (uint64_t i = 0; i < m_Images; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = m_Size;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(m_GraphicsContext.GetDevice(), 1, &descriptorWrite, 0,
				nullptr);
		}
	}

	void UniformBuffer::UpdateBuffer(void* newData, size_t size,
		uint64_t currentImage)
	{
		if (size > m_Size)
			throw std::out_of_range("size greater than buffer count");
		if (currentImage >= m_Images)
			throw std::out_of_range("currentImage greater than image count");
    
		memcpy(allocInfos[currentImage].pMappedData, newData, size);
	}

	VkBuffer UniformBuffer::GetBuffer(size_t index)
	{
		return uniformBuffers[index];
	}

	VkDescriptorSet UniformBuffer::GetDescriptorSet(size_t index)
	{
		return descriptorSets[index];
	}

	void UniformBuffer::FreeSet()
	{
		if (!boundToSet)
			return;

		vkFreeDescriptorSets(m_GraphicsContext.GetDevice(), *pDescriptorPool,
			static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());

		boundToSet = false;
	}
}