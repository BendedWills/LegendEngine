#include <LegendEngine/Graphics/Vulkan/SingleUpdateBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VkDefs.hpp>
#include <LegendEngine/IO/Logger.hpp>

namespace le
{
	SingleUpdateBuffer::SingleUpdateBuffer(VulkanGraphicsContext& context,
		const size_t vertexCount, const size_t indexCount)
		:
		m_Context(context.GetTetherGraphicsContext()),
		m_VertexCount(vertexCount),
		m_IndexCount(indexCount),
		m_VertexStager(context),
		m_IndexStager(context),
		m_TransferQueueMutex(context.GetTransferQueueMutex())
	{
		const size_t vertexBufferSize = m_VertexCount * sizeof(VertexTypes::Vertex3);
		const size_t indexBufferSize = m_IndexCount * sizeof(uint32_t);

		const auto [vertexBuffer, vertexAlloc] =
			CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize);
		const auto [indexBuffer, indexAlloc] =
			CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize);

		m_VertexBuffer = vertexBuffer;
		m_IndexBuffer = indexBuffer;
		m_VertexAllocation = vertexAlloc;
		m_IndexAllocation = indexAlloc;

		m_VertexStager.CreateStagingBuffer(m_VertexBuffer, vertexBufferSize);
		m_IndexStager.CreateStagingBuffer(m_IndexBuffer, indexBufferSize);
	}

	SingleUpdateBuffer::~SingleUpdateBuffer()
	{
		vmaDestroyBuffer(m_Context.GetAllocator(), m_VertexBuffer, m_VertexAllocation);
		vmaDestroyBuffer(m_Context.GetAllocator(), m_IndexBuffer, m_IndexAllocation);
	}

	void SingleUpdateBuffer::Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices)
	{
		bool hasUpdated = false;
		if (!m_HasUpdated.compare_exchange_strong(hasUpdated, true))
		{
			LE_ASSERT(false, "Update was called twice on a buffer made with UpdateFrequency::NEVER_UPDATES");
		}

		m_VertexStager.Upload(vertices.data(), m_VertexCount * sizeof(VertexTypes::Vertex3));
		m_IndexStager.Upload(indices.data(), m_IndexCount * sizeof(uint32_t));

	}

	void SingleUpdateBuffer::Resize(size_t vertexCount, size_t indexCount)
	{
		LE_WARN("Resize called on a vertex buffer with never update frequency. This won't affect the size.");
	}

	size_t SingleUpdateBuffer::GetVertexCount()
	{
		return m_VertexCount;
	}

	size_t SingleUpdateBuffer::GetIndexCount()
	{
		return m_IndexCount;
	}

	VulkanVertexBuffer::BufferInfo SingleUpdateBuffer::GetBufferInfo() const
	{
		return {
			.vertex = m_VertexBuffer,
			.index = m_IndexBuffer,
			.indexCount = m_IndexCount,
		};
	}

	std::pair<VkBuffer, VmaAllocation> SingleUpdateBuffer::CreateBuffer(VkBufferUsageFlags flags, const size_t size) const
	{
		VkBuffer buffer = nullptr;
		VmaAllocation allocation = nullptr;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

		LE_CHECK_VK(vmaCreateBuffer(m_Context.GetAllocator(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr));

		return std::make_pair(buffer, allocation);
	}
}
