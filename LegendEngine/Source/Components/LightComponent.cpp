#include <LegendEngine/Components/LightComponent.hpp>

namespace LegendEngine::Components
{
    void LightComponent::SetIntensity(const float intensity)
    {
        m_Color.w = intensity;
    }

    void LightComponent::SetColor(const Color& color)
    {
        m_Color = color;
    }

    void LightComponent::SetLightType(const LightType lightType)
    {
        m_LightType = lightType;
    }

    float LightComponent::GetIntensity() const
    {
        return m_Color.w;
    }

    Color LightComponent::GetColor() const
    {
        return m_Color;
    }

    LightComponent::LightType LightComponent::GetLightType() const
    {
        return m_LightType;
    }
}
