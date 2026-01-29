#pragma once

#include <LegendEngine/Resources/Shader.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    class ShaderModule final
    {
    public:
        ShaderModule(Tether::Rendering::Vulkan::GraphicsContext& context,
            const Resources::Shader::Stage& stage);
        ShaderModule(ShaderModule&& other) noexcept;
        ShaderModule(const ShaderModule&) = delete;
        ~ShaderModule();

        VkShaderModule Get() const;
    private:
        bool m_Moved = false;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        VkShaderModule m_ShaderModule = nullptr;
    };
}
