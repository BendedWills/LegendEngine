#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Material.hpp>

namespace LegendEngine::Resources
{
    Material::Material()
        :
        m_ShaderManager(Application::Get().GetGraphicsContext().GetShaderManager()),
        m_pShader(m_ShaderManager.GetByID("solid"))
    {}

    void Material::SetTexture(Texture2D* toSet)
    {
        m_pTexture = toSet;

        const Graphics::ShaderManager& manager =
            Application::Get().GetGraphicsContext().GetShaderManager();
        if (toSet)
            m_pShader = manager.GetByID("textured");
        else
            m_pShader = manager.GetByID("solid");

        m_Changed = true;
    }

    void Material::SetColor(const Color& toSet)
    {
        m_Color = toSet;
        m_Changed = true;
    }

    Texture2D* Material::GetTexture() const
    {
        return m_pTexture;
    }

    Shader* Material::GetShader() const
    {
        return m_pShader;
    }

    Color Material::GetColor() const
    {
        return m_Color;
    }

    void Material::SetChanged(const bool changed)
    {
        m_Changed = changed;
    }

    bool Material::HasChanged() const
    {
        return m_Changed;
    }

    Scope<Material> Material::Create()
    {
        LGENG_DEBUG_LOG("Material created");
        return Application::Get().GetGraphicsContext().CreateMaterial();
    }
}