#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Tether::Rendering::Vulkan;

    ShaderModule::ShaderModule(GraphicsContext& context,
        const Resources::Shader::Stage& stage)
        :
        m_Context(context)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = stage.compiledShaderSize;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(
            stage.compiledShaderCode);

        if (vkCreateShaderModule(m_Context.GetDevice(), &createInfo, nullptr,
            &m_ShaderModule) != VK_SUCCESS)
            throw std::runtime_error("Failed to create shader module");
    }

    ShaderModule::ShaderModule(ShaderModule&& other) noexcept
        :
        m_Context(other.m_Context),
        m_ShaderModule(other.m_ShaderModule)
    {
        other.m_Moved = true;
    }

    ShaderModule::~ShaderModule()
    {
        if (m_Moved)
            return;

        vkDestroyShaderModule(m_Context.GetDevice(), m_ShaderModule, nullptr);
    }

    VkShaderModule ShaderModule::Get() const
    {
        return m_ShaderModule;
    }
}
