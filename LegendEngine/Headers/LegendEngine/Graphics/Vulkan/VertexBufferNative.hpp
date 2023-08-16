#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer;
	class VertexBufferNative : public LegendEngine::IVertexBufferNative
	{
		friend VulkanRenderer;
	public:
		LEGENDENGINE_NO_COPY(VertexBufferNative);
		
		VertexBufferNative(TetherVulkan::GraphicsContext& context,
			VertexBuffer* pVertexBuffer);
		~VertexBufferNative();
	protected:
		VkBuffer vertexBuffer;
		VmaAllocation allocation;
		VkBuffer indexBuffer;
		VmaAllocation indexAlloc;

		VmaAllocator m_Allocator = nullptr;
		TetherVulkan::GraphicsContext& m_Context;
	private:
		bool OnCreate(
			VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
			uint32_t* pIndices, uint64_t indexCount
		);
		
		void UploadData(void* data, uint64_t dataBytes, VkBuffer buffer);
	};
}
