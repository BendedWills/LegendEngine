#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Graphics/Vulkan/VertexBufferNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

namespace LegendEngine::Vulkan
{
	VertexBufferNative::VertexBufferNative(TetherVulkan::GraphicsContext& context,
		VertexBuffer* pVertexBuffer)
		:
		IVertexBufferNative(pVertexBuffer),
		m_Context(context),
		m_Allocator(context.GetAllocator())
	{}

	VertexBufferNative::~VertexBufferNative()
	{
		vkDeviceWaitIdle(m_Context.GetDevice());

		vmaDestroyBuffer(m_Allocator, vertexBuffer, allocation);
		vmaDestroyBuffer(m_Allocator, indexBuffer, indexAlloc);
	}

	bool VertexBufferNative::OnCreate(
		VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
		uint32_t* pIndices, uint64_t indexCount
	)
	{
		if (!pVertices || vertexCount == 0 || !pIndices || indexCount == 0)
			return false;

		// Create vertex buffer
		uint64_t verticesSize = sizeof(pVertices[0]) * vertexCount;
		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.size = verticesSize;
			createInfo.usage =
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
				VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
				VMA_ALLOCATION_CREATE_MAPPED_BIT;

			if (vmaCreateBuffer(m_Allocator, &createInfo,
				&allocInfo, &vertexBuffer, &allocation, nullptr) != VK_SUCCESS)
				return false;

#if !defined(NDEBUG)
			vmaSetAllocationName(m_Allocator, allocation, "Vertex buffer");
#endif
		}

		// Create index buffer
		uint64_t indicesSize = sizeof(pIndices[0]) * indexCount;
		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.size = indicesSize;
			createInfo.usage =
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT
				| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
				VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
				VMA_ALLOCATION_CREATE_MAPPED_BIT;

			if (vmaCreateBuffer(m_Allocator, &createInfo,
				&allocInfo, &indexBuffer, &indexAlloc, nullptr) != VK_SUCCESS)
				return false;

#if !defined(NDEBUG)
			vmaSetAllocationName(m_Allocator, indexAlloc, "Index buffer");
#endif
		}

		TetherVulkan::BufferStager verticesStager(m_Context, vertexBuffer, verticesSize);
		TetherVulkan::BufferStager indicesStager(m_Context, indexBuffer, indicesSize);
		verticesStager.UploadDataAsync(pVertices);
		indicesStager.UploadDataAsync(pIndices);

		verticesStager.Wait();
		indicesStager.Wait();

		// TODO: Log Initialized vertex buffer

		return true;
	}
}
