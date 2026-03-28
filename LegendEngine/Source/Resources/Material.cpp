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
        m_ShaderManager(Application::Get().GetGraphicsResources().GetShaderManager()),
        m_shaderId(m_ShaderManager.GetByID("solid"))
    {
        m_buffer = m_context.CreatePerFrameBuffer(Buffer::Usage::UNIFORM_BUFFER,
            sizeof(Uniforms));

        GraphicsResources& resources = Application::Get().GetGraphicsResources();

        DynamicUniforms::DescriptorInfo infos[] =
        {
            {
                DescriptorType::UNIFORM_BUFFER,
                DynamicUniforms::UpdateFrequency::PER_FRAME,
                1,
                &resources.GetMaterialLayout()
            },
            {
                DescriptorType::COMBINED_IMAGE_SAMPLER,
                DynamicUniforms::UpdateFrequency::OCCASIONAL,
                1,
                &resources.GetMaterialLayout()
            },
        };

        m_uniforms = m_context.CreateDynamicUniforms(std::span(infos));
    }

    void Material::SetTexture(const ID<Texture>& toSet)
    {
        m_textureId = toSet;

        if (toSet != 0)
            m_shaderId = m_ShaderManager.GetByID("textured");
        else
            m_shaderId = m_ShaderManager.GetByID("solid");

        m_Changed = true;
    }

    void Material::SetColor(const Color& toSet)
    {
        m_uniformData.color = toSet;
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

    void Material::UpdateUniforms()
    {
        m_uniforms->UpdateBuffer(*m_buffer, 0);

        // TODO:
        // if (m_textureId != 0)
        //     m_uniforms->UpdateCombinedImageSampler(1);

        m_Changed = false;
    }

    void Material::CopyUniformData()
    {
        m_buffer->Update(sizeof(m_uniformData), &m_uniformData);
    }

    DynamicUniforms& Material::GetUniforms() const
    {
        return *m_uniforms;
    }
}
