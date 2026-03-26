#pragma once

#include <VulkanBufferStager.hpp>
#include <VulkanVertexBuffer.hpp>

namespace le
{
	class SingleUpdateBuffer : public VulkanVertexBuffer
	{
	public:
		SingleUpdateBuffer(VulkanGraphicsContext& context,
			size_t vertexCount, size_t indexCount);
		~SingleUpdateBuffer() override;

		void Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices) override;
		void Resize(size_t vertexCount, size_t indexCount) override;
		size_t GetVertexCount() override;
		size_t GetIndexCount() override;

		[[nodiscard]] BufferInfo GetBufferInfo() const override;
	private:
		[[nodiscard]] std::pair<VkBuffer, VmaAllocation> CreateBuffer(VkBufferUsageFlags flags, size_t size) const;

		TetherVulkan::GraphicsContext& m_Context;

		VkBuffer m_VertexBuffer = nullptr;
		VkBuffer m_IndexBuffer = nullptr;
		VmaAllocation m_VertexAllocation = nullptr;
		VmaAllocation m_IndexAllocation = nullptr;

		size_t m_VertexCount = 0;
		size_t m_IndexCount = 0;

		VulkanBufferStager m_VertexStager;
		VulkanBufferStager m_IndexStager;

		std::atomic_bool m_HasUpdated = false;

		std::mutex& m_TransferQueueMutex;
	};
}