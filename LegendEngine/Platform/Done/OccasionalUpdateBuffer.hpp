#pragma once

#include <../Vulkan/VulkanVertexBuffer.hpp>
#include <Tether/Rendering/Vulkan/BufferStager.hpp>

#include <span>
#include <../Vulkan/VulkanBufferStager.hpp>

namespace le
{
	class OccasionalUpdateBuffer : public VulkanVertexBuffer
	{
	public:
		OccasionalUpdateBuffer(VulkanGraphicsContext& context);
		~OccasionalUpdateBuffer() override;

		void Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices) override;
		void Resize(size_t vertexCount, size_t indexCount) override;
		size_t GetVertexCount() override;
		size_t GetIndexCount() override;

		// Called on rendering, by the main thread
		void DeleteUnusedBuffers(uint32_t currentFrame) override;
		void Use(uint32_t currentFrame) override;

		[[nodiscard]] BufferInfo GetBufferInfo() const override;

		bool ShouldWait() override;
		void DeleteStager() override;

		VkSemaphore GetSemaphore() const override;
		size_t StartWait() override;
	private:
		struct BufferDesc
		{
			std::atomic<VkBuffer> vertexBuffer = nullptr;
			std::atomic<VkBuffer> indexBuffer = nullptr;
			std::atomic<VmaAllocation> vertexAllocation = nullptr;
			std::atomic<VmaAllocation> indexAllocation = nullptr;

			size_t vertexCount = 0;
			size_t indexCount = 0;
		};

		void CreateSemaphore();
		[[nodiscard]] std::pair<VkBuffer, VmaAllocation> CreateBuffer(VkBufferUsageFlags flags, size_t size) const;
		BufferDesc* AcquireUnusedBuffer();

		void DestroyBuffer(BufferDesc& buffer) const;

		BufferDesc m_Buffer1;
		BufferDesc m_Buffer2;

		Tether::Rendering::Vulkan::GraphicsContext& m_Context;

		std::atomic<VkSemaphore> m_Semaphore = nullptr;
		std::atomic_size_t m_SemaphoreValue = 0;

		std::mutex& m_TransferQueueMutex;
		std::mutex m_UpdateMutex;
		std::atomic<BufferDesc*> m_CurrentBuffer = nullptr;
		std::atomic_bool m_HasUpdated = false;
		std::atomic_bool m_HasStagerBeenDeleted = true;
		std::atomic_bool m_HasUpdatedSinceWaiting = false;
		uint64_t m_FramesUsingBuffer = 0;

		VulkanBufferStager m_VertexStager;
		VulkanBufferStager m_IndexStager;
	};
}