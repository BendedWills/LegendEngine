#ifndef _LEGENDENGINE_VULKAN_MATERIALNATIVE_HPP
#define _LEGENDENGINE_VULKAN_MATERIALNATIVE_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Resources/Material.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class MaterialNative : public Resources::IMaterialNative
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_NO_COPY(MaterialNative);
        
        MaterialNative(VulkanRenderer* pRenderer, Resources::Material* pMaterial)
            :
            pVulkanRenderer(pRenderer),
            Resources::IMaterialNative(pMaterial)
        {}
    protected:
        VkSampler sampler;

        VulkanRenderer* pVulkanRenderer = nullptr;
    private:
		bool OnCreate();
		void OnUpdate();
		void OnDispose();
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_MATERIALNATIVE_HPP