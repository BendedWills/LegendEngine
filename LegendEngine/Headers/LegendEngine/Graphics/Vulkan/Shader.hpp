#ifndef _LEGENDENGINE_VULKAN_SHADER_HPP
#define _LEGENDENGINE_VULKAN_SHADER_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/Shader.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class Shader : public LegendEngine::Shader
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(Shader);
        LEGENDENGINE_NO_COPY(Shader);

        Shader(VulkanRenderer* pRenderer)
            :
            pVulkanRenderer(pRenderer),
            LegendEngine::Shader(pRenderer, RenderingAPI::VULKAN)
        {}
    protected:
        bool OnShaderCreate(ShaderStage* pStages, uint64_t stageCount,
            UniformDesc* pDescs, uint64_t uniformCount);
        void OnShaderDispose();

        // TODO

        uint64_t verticesSize = 0;
        
        VulkanRenderer* pVulkanRenderer = nullptr;
    };
}

#endif //_LEGENDENGINE_VULKAN_SHADER_HPP