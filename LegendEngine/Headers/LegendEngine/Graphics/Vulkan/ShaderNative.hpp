#ifndef _LEGENDENGINE_VULKAN_SHADER_HPP
#define _LEGENDENGINE_VULKAN_SHADER_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/Shader.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class ShaderNative : public IShaderNative
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_NO_COPY(ShaderNative);

        ShaderNative(VulkanRenderer* pRenderer, Shader* pShader)
            :
            pVulkanRenderer(pRenderer),
            IShaderNative(pShader)
        {}
    protected:
        bool OnCreate(ShaderStage* pStages, uint64_t stageCount);
        void OnDispose();

        // TODO

        uint64_t verticesSize = 0;
        
        VulkanRenderer* pVulkanRenderer = nullptr;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SHADER_HPP