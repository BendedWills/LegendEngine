#ifndef _LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP
#define _LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
    class VertexBuffer : public LegendEngine::VertexBuffer
    {
        friend VulkanRenderer;
    public:
        VertexBuffer(VulkanRenderer* pRenderer)
            :
            pVulkanRenderer(pRenderer),
            LegendEngine::VertexBuffer(pRenderer, VertexBufferType::VULKAN) 
        {}
        
        VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) = delete;
    protected:
        bool OnBufferCreate(VertexTypes::Vertex2* pVertices, 
            uint64_t vertexCount);
        void OnBufferDispose();

        bool FindMemoryType(uint32_t typeFilter, 
            VkMemoryPropertyFlags properties, uint32_t* pMemoryType);

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VulkanRenderer* pVulkanRenderer = nullptr;
    };
}

#endif //_LEGENDENGINE_VULKAN_VERTEXBUFFER_HPP