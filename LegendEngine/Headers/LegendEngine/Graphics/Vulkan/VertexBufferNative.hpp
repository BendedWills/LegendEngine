#ifndef _LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP
#define _LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>

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
        
        VertexBufferNative(VulkanRenderer* pRenderer, VertexBuffer* pVertexBuffer)
            :
            pVulkanRenderer(pRenderer),
            IVertexBufferNative(pVertexBuffer)
        {}
    protected:
        VkBuffer vertexBuffer;
        VmaAllocation allocation;
		VkBuffer indexBuffer;
		VmaAllocation indexAlloc;

        VulkanRenderer* pVulkanRenderer = nullptr;
    private:
		bool OnCreate(
			VertexTypes::Vertex3* pVertices, uint64_t vertexCount,
			uint32_t* pIndices, uint64_t indexCount
        );
		void OnDispose();

		bool UploadData(void* data, uint64_t dataBytes, VkBuffer buffer);
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP