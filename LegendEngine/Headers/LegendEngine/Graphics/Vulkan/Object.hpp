#ifndef _LEGENDENGINE_VULKAN_OBJECT_HPP
#define _LEGENDENGINE_VULKAN_OBJECT_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/Shader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Objects/Object.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class Object : public LegendEngine::Objects::Object
    {
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(Object);
        LEGENDENGINE_NO_COPY(Object);

        Object(VulkanRenderer* pRenderer)
            :
            pVulkanRenderer(pRenderer),
            LegendEngine::Object(pRenderer, RenderingAPI::VULKAN)
        {}
    protected:
        bool OnObjectCreate();
        void OnObjectDispose();

        uint64_t verticesSize = 0;
    };
}

#endif //_LEGENDENGINE_VULKAN_OBJECT_HPP