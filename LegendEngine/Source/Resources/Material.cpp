#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Material.hpp>

namespace LegendEngine::Resources
{
    void Material::SetTexture(Texture2D* toSet)
    {
        m_pTexture = toSet;
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
        return Application::Get().GetGraphicsContext().CreateMaterial();
    }
}