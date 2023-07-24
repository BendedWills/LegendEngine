#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/UniformManager.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer;
	class UniformBuffer
	{
	public:
		UniformBuffer(TetherVulkan::GraphicsContext& context, size_t size, uint32_t images);
		~UniformBuffer();
		LEGENDENGINE_NO_COPY(UniformBuffer);

		void BindToSet(UniformManager* manager, VkDescriptorSetLayout layout);
		void BindToSet(VkDescriptorPool* pool, VkDescriptorSetLayout layout);
		void Bind(uint32_t binding);

		/**
		 * @brief Fills the uniform buffer with newData. 
		 *  This uniform buffer must be passed
		 *  to a pipeline class before this function is used.
		 */
		void UpdateBuffer(void* newData, size_t size, uint64_t currentImage);

		VkBuffer GetBuffer(size_t index);
		VkDescriptorSet GetDescriptorSet(size_t index);
	private:
		void FreeSet();
		
		TetherVulkan::GraphicsContext& m_GraphicsContext;
		
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VmaAllocation> allocations;
		std::vector<VmaAllocationInfo> allocInfos;
		std::vector<VkBuffer> uniformBuffers;

		size_t m_Size = 0;
		uint32_t m_Images = 0;

		VkDescriptorPool* pDescriptorPool = nullptr;
		bool boundToSet = false;
	};
}
