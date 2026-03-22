#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/DynamicUniforms.hpp>
#include <LE/Math/Types.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
    class Shader;

    Material::Material()
        :
        m_context(Application::Get().GetGraphicsContext()),
        m_ShaderManager(m_context.GetShaderManager()),
        m_shaderId(m_ShaderManager.GetByID("solid")->id)
    {
        m_buffer = m_context.CreatePerFrameBuffer(Buffer::Usage::UNIFORM_BUFFER,
            sizeof(Uniforms));

        DynamicUniforms::DescriptorInfo infos[] =
        {
            {
                DynamicUniforms::DescriptorType::UNIFORM_BUFFER,
                DynamicUniforms::UpdateFrequency::PER_FRAME,
                1
            },
            {
                DynamicUniforms::DescriptorType::COMBINED_IMAGE_SAMPLER,
                DynamicUniforms::UpdateFrequency::OCCASIONAL,
                1
            },
        };

        m_uniforms = m_context.CreateDynamicUniforms(std::span(infos));
    }

    void Material::SetTexture(const ID<Texture>& toSet)
    {
        m_textureId = toSet;

        const ShaderManager& manager =
            Application::Get().GetGraphicsContext().GetShaderManager();
        if (toSet != 0)
            m_shaderId = manager.GetByID("textured")->id;
        else
            m_shaderId = manager.GetByID("solid");

        m_Changed = true;
    }

    void Material::SetColor(const Color& toSet)
    {
        m_uniformData.color = toSet;
        m_Changed = true;
    }

    Resource::ID<Texture> Material::GetTexture() const
    {
        return m_textureId;
    }

    Resource::ID<Shader> Material::GetShader() const
    {
        return m_shaderId;
    }

    Color Material::GetColor() const
    {
        return m_uniformData.color;
    }

    bool Material::HasChanged() const
    {
        return m_Changed;
    }

    void Material::UpdateUniforms(const uint32_t currentFrame)
    {
        m_uniforms->m_currentFrame = currentFrame;

        for (uint32_t i = 0; i < Application::FRAMES_IN_FLIGHT; i++)
            *static_cast<Uniforms*>(m_buffer->GetMappedDataForFrame(i)) = m_uniformData;

        m_uniforms->UpdateBuffer(*m_buffer, 0);

        if (m_textureId != 0)
            m_uniforms->UpdateCombinedImageSampler(1);

        m_Changed = false;
    }

    DynamicUniforms& Material::GetUniforms() const
    {
        return *m_uniforms;
    }
}
