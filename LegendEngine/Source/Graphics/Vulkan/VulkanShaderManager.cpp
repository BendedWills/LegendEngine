#include <LegendEngine/Graphics/Vulkan/VulkanShaderManager.hpp>

#include <Assets/CompiledShaders/solid.vert.spv.h>
#include <Assets/CompiledShaders/solid.frag.spv.h>
#include <Assets/CompiledShaders/textured.vert.spv.h>
#include <Assets/CompiledShaders/textured.frag.spv.h>

namespace LegendEngine::Graphics::Vulkan
{
    using namespace Resources::VulkanShaders;

#define STAGES(id) \
    static Resources::Shader::Stage STAGES_##id[] = { \
        { ShaderType::VERTEX, _binary_##id##_vert_spv, \
            sizeof(_binary_##id##_vert_spv) }, \
        { ShaderType::FRAG, _binary_##id##_frag_spv, \
            sizeof(_binary_##id##_frag_spv) } \
    }

    STAGES(solid);
    STAGES(textured);

    VulkanShaderManager::VulkanShaderManager(TetherVulkan::GraphicsContext& graphicsContext,
        const std::span<VkDescriptorSetLayout>& setLayouts)
        :
        m_GraphicsContext(graphicsContext),
        m_SetLayouts(setLayouts),
        m_SolidShader(m_GraphicsContext, STAGES_solid, m_SetLayouts),
        m_TexturedShader(m_GraphicsContext, STAGES_textured, m_SetLayouts)
    {
        RegisterShader("solid", m_SolidShader);
        RegisterShader("textured", m_TexturedShader);
    }
}
