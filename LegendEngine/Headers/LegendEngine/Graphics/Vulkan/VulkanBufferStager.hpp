#pragma once

#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>

namespace le
{
	class VulkanBufferStager final
	{
	public:
		explicit VulkanBufferStager(VulkanGraphicsContext& context);
		~VulkanBufferStager();
		LEGENDENGINE_NO_COPY(VulkanBufferStager);

		void CreateStagingBuffer(VkBuffer target, size_t targetSize);
		void Upload(const void* data, size_t targetSize,
			VkSemaphore signalSemaphore = nullptr, size_t semaphoreValue = 0) const;
		void DeleteStagingBuffer();
	private:
		void Wait() const;
		void CreateCommandBuffer();
		void RecordCommandBuffer(VkBuffer target, size_t targetSize) const;
		void CreateFence();

		TetherVulkan::GraphicsContext& m_GraphicsContext;
		std::mutex& m_TransferMutex;
		VkDevice m_Device = nullptr;
		VkQueue m_Queue = nullptr;
		VkCommandPool m_CommandPool = nullptr;
		VkCommandBuffer m_CommandBuffer = nullptr;

		VkBuffer m_StagingBuffer = nullptr;
		VmaAllocationInfo m_StagingInfo;
		VmaAllocation m_StagingAllocation = nullptr;
		VkFence m_Fence = nullptr;
	};
}
