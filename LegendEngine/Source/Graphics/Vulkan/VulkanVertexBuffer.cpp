#include <LegendEngine/Common/Assert.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanVertexBuffer.hpp>

namespace le
{
    using namespace Tether::Rendering::Vulkan;

    VulkanVertexBuffer::VulkanVertexBuffer(
            VulkanGraphicsContext& context,
            size_t initialVertexCount, size_t initialIndexCount
        )
        :
        m_VertexCount(initialVertexCount),
        m_IndexCount(initialIndexCount),
		m_Context(context.GetTetherGraphicsContext()),
		m_Mutex(context.GetTransferQueueMutex())
    {
        size_t vertexBufferSize = initialVertexCount * sizeof(VertexTypes::Vertex3);
        size_t indexBufferSize = initialIndexCount * sizeof(uint32_t);

        CreateVertexBuffer(vertexBufferSize);
        CreateIndexBuffer(indexBufferSize);

        m_VertexStager.emplace(
            m_Context,
            m_VertexBuffer,
            vertexBufferSize,
            context.GetTransferQueue(),
            context.GetTransferPool()
        );

        m_IndexStager.emplace(
            m_Context,
            m_IndexBuffer,
            indexBufferSize,
            context.GetTransferQueue(),
			context.GetTransferPool()
        );
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        DestroyBuffers();
    }

    void VulkanVertexBuffer::Update(const std::span<VertexTypes::Vertex3> vertices, const std::span<uint32_t> indices)
    {
        LE_ASSERT(vertices.size() <= m_VertexCount, "Vertex buffer size mismatch");
        LE_ASSERT(indices.size() <= m_IndexCount, "Vertex buffer size mismatch");

    	m_FinishedUploading = false;

    	std::scoped_lock lock(m_Mutex);

    	m_VertexStager->UploadDataAsync(vertices.data());
    	m_IndexStager->UploadDataAsync(indices.data());

    	m_VertexStager->Wait();
    	m_IndexStager->Wait();
    }

    void VulkanVertexBuffer::Resize(const size_t vertexCount, const size_t indexCount)
    {
    	const size_t vertexBufferSize = vertexCount * sizeof(VertexTypes::Vertex3);
    	const size_t indexBufferSize = indexCount * sizeof(uint32_t);

        DestroyBuffers();
    	CreateVertexBuffer(vertexBufferSize);
    	CreateIndexBuffer(indexBufferSize);
    }

    size_t VulkanVertexBuffer::GetVertexCount()
    {
        return m_VertexCount;
    }

    size_t VulkanVertexBuffer::GetIndexCount()
    {
        return m_IndexCount;
    }

    VkBuffer VulkanVertexBuffer::GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

    VkBuffer VulkanVertexBuffer::GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }

    void VulkanVertexBuffer::Wait() const
    {
		if (m_FinishedUploading)
			return;

		m_VertexStager->Wait();
		m_IndexStager->Wait();
	}

	void VulkanVertexBuffer::FinishDataUpload()
	{
		if (m_FinishedUploading)
			return;

		Wait();

		m_VertexStager.reset();
		m_IndexStager.reset();

		m_FinishedUploading = true;
	}

	void VulkanVertexBuffer::CreateVertexBuffer(const size_t size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage =
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

		if (vmaCreateBuffer(m_Context.GetAllocator(), &bufferInfo,
			&allocInfo, &m_VertexBuffer, &m_VertexAllocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex buffer");
	}

	void VulkanVertexBuffer::CreateIndexBuffer(const size_t size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage =
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

		if (vmaCreateBuffer(m_Context.GetAllocator(), &bufferInfo,
			&allocInfo, &m_IndexBuffer, &m_IndexAllocation, nullptr) != VK_SUCCESS)
		{
			vmaDestroyBuffer(m_Context.GetAllocator(), m_VertexBuffer, m_VertexAllocation);
			throw std::runtime_error("Failed to create index buffer");
		}
	}

	void VulkanVertexBuffer::DestroyBuffers()
	{
    	Wait();

		m_Context.GetDeviceLoader().vkDeviceWaitIdle(m_Context.GetDevice());

		m_VertexStager.reset();
		m_IndexStager.reset();

		vmaDestroyBuffer(m_Context.GetAllocator(), m_VertexBuffer, m_VertexAllocation);
		vmaDestroyBuffer(m_Context.GetAllocator(), m_IndexBuffer, m_IndexAllocation);
	}
}