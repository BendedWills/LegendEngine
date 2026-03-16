#include <LegendEngine/Common/Assert.hpp>
#include <LegendEngine/Graphics/Vulkan/OccasionalUpdateBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VkDefs.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>

namespace le
{
    using namespace Tether::Rendering::Vulkan;

    OccasionalUpdateBuffer::OccasionalUpdateBuffer(
	    VulkanGraphicsContext& context)
	    :
	    m_Context(context.GetTetherGraphicsContext()),
	    m_TransferQueueMutex(context.GetTransferQueueMutex()),
	    m_VertexStager(context),
	    m_IndexStager(context)
    {}

    OccasionalUpdateBuffer::~OccasionalUpdateBuffer()
    {
	    DestroyBuffer(m_Buffer1);
	    DestroyBuffer(m_Buffer2);
    }

    void OccasionalUpdateBuffer::Update(const std::span<VertexTypes::Vertex3> vertices, const std::span<uint32_t> indices)
    {
    	BufferDesc* buffer = AcquireUnusedBuffer();

    	std::scoped_lock lock(m_UpdateMutex);

	    const size_t vertexBufferSize = vertices.size() * sizeof(VertexTypes::Vertex3);
	    const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	    const auto [vertexBuffer, vertexAlloc] =
		    CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize);
	    const auto [indexBuffer, indexAlloc] =
		    CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize);

	    buffer->vertexBuffer = vertexBuffer;
	    buffer->vertexAllocation = vertexAlloc;
	    buffer->indexBuffer = indexBuffer;
	    buffer->indexAllocation = indexAlloc;
	    buffer->vertexCount = vertices.size();
	    buffer->indexCount = indices.size();

	    m_VertexStager.CreateStagingBuffer(buffer->vertexBuffer, vertexBufferSize);
	    m_IndexStager.CreateStagingBuffer(buffer->indexBuffer, indexBufferSize);
	    m_VertexStager.Upload(vertices.data(), vertexBufferSize);
	    m_IndexStager.Upload(indices.data(), indexBufferSize);

	    m_CurrentBuffer = buffer;
	    m_HasUpdated = true;
    }

    void OccasionalUpdateBuffer::Resize(const size_t, const size_t)
    {
	    LE_WARN("Resize called on a vertex buffer with occasional update frequency. This won't affect the size until Update is called");
    }

    size_t OccasionalUpdateBuffer::GetVertexCount()
    {
	    return m_CurrentBuffer.load()->vertexCount;
    }

    size_t OccasionalUpdateBuffer::GetIndexCount()
    {
	    return m_CurrentBuffer.load()->indexCount;
    }

    void OccasionalUpdateBuffer::DeleteUnusedBuffers(const std::vector<VkFence>& fences, const uint32_t currentFrame)
    {
	    const BufferDesc* currentBuffer = m_CurrentBuffer.load();
    	if (!currentBuffer)
    		return;

		if (m_VertexStager.IsSignaled() && m_IndexStager.IsSignaled())
		{
			std::scoped_lock lock(m_UpdateMutex);

			// It might have changed
			if (m_VertexStager.IsSignaled() && m_IndexStager.IsSignaled())
			{
				m_VertexStager.DeleteStagingBuffer();
				m_IndexStager.DeleteStagingBuffer();
			}
		}

    	// If no updates occurred, there's no point checking this next part
	    if (!m_HasUpdated)
		    return;

	    BufferDesc* other = currentBuffer == &m_Buffer1 ? &m_Buffer2 : &m_Buffer1;

    	// Check the fences to make sure no frames are using the other buffer
	    for (const VkFence fence : fences)
		    if (vkGetFenceStatus(m_Context.GetDevice(), fence) != VK_SUCCESS)
			    return;

    	// At this point, the new buffer has been created by Update (possibly
    	// on another thread) and the old one isn't in use by frames in flight,
    	// so it can be deleted.
	    DestroyBuffer(*other);

    	// Signal that nothing has been updated again
    	m_HasUpdated = false;
    }

    VkBuffer OccasionalUpdateBuffer::GetVertexBuffer() const
    {
    	if (!m_CurrentBuffer)
    		return nullptr;

	    return m_CurrentBuffer.load()->vertexBuffer;
    }

    VkBuffer OccasionalUpdateBuffer::GetIndexBuffer() const
    {
    	if (!m_CurrentBuffer)
    		return nullptr;

	    return m_CurrentBuffer.load()->indexBuffer;
    }

	std::pair<VkBuffer, VmaAllocation> OccasionalUpdateBuffer::CreateBuffer(VkBufferUsageFlags flags, const size_t size) const
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

	OccasionalUpdateBuffer::BufferDesc* OccasionalUpdateBuffer::AcquireUnusedBuffer()
	{
		while (true)
		{
			BufferDesc* buffer = &m_Buffer1;
			if (!buffer->vertexBuffer)
				return buffer;

			buffer = &m_Buffer2;
			if (!buffer->vertexBuffer)
				return buffer;
		}
	}

	void OccasionalUpdateBuffer::DestroyBuffer(BufferDesc& buffer)
	{
    	if (!buffer.vertexBuffer)
    		return;

    	std::scoped_lock lock(m_UpdateMutex);

    	// TODO: figure out why vulkan gets mad about this
    	m_VertexStager.Wait();
    	m_IndexStager.Wait();

		vmaDestroyBuffer(m_Context.GetAllocator(), buffer.vertexBuffer, buffer.vertexAllocation);
		vmaDestroyBuffer(m_Context.GetAllocator(), buffer.indexBuffer, buffer.indexAllocation);

    	// This shows the buffer has been destroyed.
    	// AcquireUnusedBuffer uses the vertex buffer to check if it's okay to
    	// create a new one since it is atomic. Once this happens, a new buffer
    	// may be created at any instant.
		buffer.vertexBuffer = nullptr;
	}
}
