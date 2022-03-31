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

        uint64_t verticesSize = 0;
        
        VulkanRenderer* pVulkanRenderer = nullptr;
    private:
		bool OnCreate(VertexTypes::Vertex2* pVertices,
			uint64_t vertexCount);
		void OnDispose();

		bool UploadVertexData(void* pVertices);
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP