#include <OccasionalUpdateBuffer.hpp>
#include <VkDefs.hpp>
#include <VulkanGraphicsContext.hpp>
#include <LE/Common/Assert.hpp>

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
    {
    	LE_ASSERT(m_Context.GetFramesInFlight() <= 64, "Frames in flight can't be greater than 64. Please ask yourself why on earth you are doing this anyways.");
    	CreateSemaphore();
    }

    OccasionalUpdateBuffer::~OccasionalUpdateBuffer()
    {
	    DestroyBuffer(m_Buffer1);
	    DestroyBuffer(m_Buffer2);

    	vkDestroySemaphore(m_Context.GetDevice(), m_Semaphore, nullptr);
    }

	void OccasionalUpdateBuffer::CreateSemaphore()
    {
    	VkSemaphoreTypeCreateInfo typeInfo{};
    	typeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    	typeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

    	VkSemaphoreCreateInfo semaphoreCreateInfo{};
    	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    	semaphoreCreateInfo.pNext = &typeInfo;

    	VkSemaphore semaphore = nullptr;

    	LE_CHECK_VK(vkCreateSemaphore(m_Context.GetDevice(), &semaphoreCreateInfo, nullptr, &semaphore));

    	m_Semaphore = semaphore;
    }

    void OccasionalUpdateBuffer::Update(const std::span<VertexTypes::Vertex3> vertices, const std::span<uint32_t> indices)
    {
    	if (vertices.empty() || indices.empty())
    		return;

    	BufferDesc* buffer = AcquireUnusedBuffer();

		// It's possible that another update happens here and then this locks
    	std::scoped_lock lock(m_UpdateMutex);

    	// Check if another update already got to this buffer
    	if (buffer->vertexBuffer)
    		return;

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

    	if (!m_Semaphore)
    		CreateSemaphore();

    	if (!m_HasStagerBeenDeleted)
    	{
			VkSemaphoreSignalInfo signalInfo{};
    		signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    		signalInfo.semaphore = m_Semaphore;
    		signalInfo.value = ++m_SemaphoreValue;

    		vkSignalSemaphore(m_Context.GetDevice(), &signalInfo);
    	}

	    m_VertexStager.Upload(vertices.data(), vertexBufferSize, m_Semaphore, ++m_SemaphoreValue);
	    m_IndexStager.Upload(indices.data(), indexBufferSize, m_Semaphore, ++m_SemaphoreValue);

    	// Relaxed because of the update mutex lock
	    m_CurrentBuffer.store(buffer, std::memory_order_relaxed);
	    m_HasUpdated.store(true, std::memory_order_relaxed);
	    m_HasStagerBeenDeleted.store(false, std::memory_order_relaxed);
    	m_HasUpdatedSinceWaiting.store(true, std::memory_order_relaxed);
    }

    void OccasionalUpdateBuffer::Resize(const size_t, const size_t)
    {
	    LE_WARN("Resize called on a vertex buffer with occasional update frequency. This won't affect the size until Update is called");
    }

    size_t OccasionalUpdateBuffer::GetVertexCount()
    {
    	if (!m_CurrentBuffer)
    		return 0;

	    return m_CurrentBuffer.load()->vertexCount;
    }

    size_t OccasionalUpdateBuffer::GetIndexCount()
    {
    	if (!m_CurrentBuffer)
    		return 0;

	    return m_CurrentBuffer.load()->indexCount;
    }

    void OccasionalUpdateBuffer::DeleteUnusedBuffers(const uint32_t currentFrame)
    {
	    const BufferDesc* currentBuffer = m_CurrentBuffer.load();
    	if (!currentBuffer)
    		return;

    	// If no updates occurred, there's no point checking this next part
	    if (!m_HasUpdated)
		    return;

    	// Check the fences to make sure no frames are using the other buffer
    	if (m_FramesUsingBuffer)
    	{
    		m_FramesUsingBuffer &= ~(1 << currentFrame);
    		return;
    	}

    	// At this point, the new buffer has been created by Update (possibly
    	// on another thread) and the old one isn't in use by frames in flight,
    	// so it can be deleted.

    	std::scoped_lock lock(m_UpdateMutex);

    	// Important to do m_CurrentBuffer.load() again here since the buffer
    	// might have changed in the time it too to get here from the top of
    	// the function. std::memory_order_relaxed is sufficient since the
    	// above mutex emits at least an acquire barrier.
    	BufferDesc* other = m_CurrentBuffer.load(std::memory_order_relaxed) == &m_Buffer1 ? &m_Buffer2 : &m_Buffer1;
	    DestroyBuffer(*other);

    	// Signal that nothing has been updated again
    	m_HasUpdated = false;
    }

    void OccasionalUpdateBuffer::Use(uint32_t currentFrame)
    {
    	m_FramesUsingBuffer |= (!m_HasUpdated) << currentFrame;
    }

    VulkanVertexBuffer::BufferInfo OccasionalUpdateBuffer::GetBufferInfo() const
    {
    	const BufferDesc* pBuffer = m_CurrentBuffer.load();
    	if (!pBuffer)
    		return {};

    	return {
    		.vertex = pBuffer->vertexBuffer,
    		.index = pBuffer->indexBuffer,
    		.indexCount = pBuffer->indexCount
    	};
    }

   	bool OccasionalUpdateBuffer::ShouldWait()
    {
	    return !m_HasStagerBeenDeleted;
    }

    void OccasionalUpdateBuffer::DeleteStager()
    {
    	std::scoped_lock lock(m_UpdateMutex);

    	// Skip deletion if another thread got to Update in the time it took
    	// to wait on deletion of the old stager it created
    	if (m_HasUpdatedSinceWaiting.load(std::memory_order_relaxed))
    	{
    		m_HasUpdatedSinceWaiting.store(false, std::memory_order_relaxed);
    		return;
    	}

    	m_VertexStager.DeleteStagingBuffer();
    	m_IndexStager.DeleteStagingBuffer();

    	vkDestroySemaphore(m_Context.GetDevice(), m_Semaphore, nullptr);
		m_SemaphoreValue = 0;
    	m_Semaphore = nullptr;

    	m_HasStagerBeenDeleted.store(true, std::memory_order_relaxed);
    }

    VkSemaphore OccasionalUpdateBuffer::GetSemaphore() const
    {
	    return m_Semaphore;
    }

    size_t OccasionalUpdateBuffer::StartWait()
    {
    	m_HasUpdatedSinceWaiting = false;
	    return m_SemaphoreValue;
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

	void OccasionalUpdateBuffer::DestroyBuffer(BufferDesc& buffer) const
	{
    	if (!buffer.vertexBuffer)
    		return;

		vmaDestroyBuffer(m_Context.GetAllocator(), buffer.vertexBuffer, buffer.vertexAllocation);
		vmaDestroyBuffer(m_Context.GetAllocator(), buffer.indexBuffer, buffer.indexAllocation);

    	// This shows the buffer has been destroyed.
    	// AcquireUnusedBuffer uses the vertex buffer to check if it's okay to
    	// create a new one since it is atomic. Once this happens, a new buffer
    	// may be created at any instant.
		buffer.vertexBuffer = nullptr;
	}
}
