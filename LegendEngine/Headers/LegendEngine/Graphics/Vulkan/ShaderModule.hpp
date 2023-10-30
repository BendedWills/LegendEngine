#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#ifdef SHADER_COMPILATION
#include <glslang/SPIRV/GlslangToSpv.h>
#endif

namespace LegendEngine::Vulkan
{
    class ShaderModule
    {
    public:
        ShaderModule(TetherVulkan::GraphicsContext& context, ShaderType type,
            uint32_t* spv, uint64_t spvSize);
        ~ShaderModule();
        LEGENDENGINE_NO_COPY(ShaderModule);

        ShaderType GetType();
        VkShaderModule Get();
    protected:
        void OnDispose();
    private:
        VkDevice m_Device = nullptr;

        ShaderType type;
        VkShaderModule shaderModule;
    };
}
