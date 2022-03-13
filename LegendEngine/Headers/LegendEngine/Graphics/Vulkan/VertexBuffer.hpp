#ifndef _LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP
#define _LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VertexBuffer : public LegendEngine::VertexBuffer
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(VertexBuffer);

        VertexBuffer(VulkanRenderer* pRenderer)
            :
            pVulkanRenderer(pRenderer),
            LegendEngine::VertexBuffer(pRenderer, RenderingAPI::VULKAN) 
        {}
        
        VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) = delete;
    protected:
        bool OnBufferCreate(VertexTypes::Vertex2* pVertices, 
            uint64_t vertexCount);
        void OnBufferDispose();

        bool UploadVertexData(void* pVertices);

        VkBuffer vertexBuffer;
        VmaAllocation allocation;

        uint64_t verticesSize = 0;
        
        VulkanRenderer* pVulkanRenderer = nullptr;
    };
}

#endif //_LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP