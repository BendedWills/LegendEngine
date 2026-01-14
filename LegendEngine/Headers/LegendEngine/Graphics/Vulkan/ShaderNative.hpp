#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Resources/Shader.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include "Pipeline.hpp"

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class ShaderNative : public Resources::IShaderNative
    {
        friend VulkanRenderer;
    public:
        LEGENDENGINE_NO_COPY(ShaderNative);

        ShaderNative(VulkanRenderer* pRenderer, Resources::Shader* pShader,
            std::span<Resources::ShaderStage> stages);

        Pipeline& GetPipeline();
    protected:
        void OnDispose();

        std::optional<Pipeline> shaderProgram;

        uint64_t verticesSize = 0;
        
        VulkanRenderer* pVulkanRenderer = nullptr;
    };
}
