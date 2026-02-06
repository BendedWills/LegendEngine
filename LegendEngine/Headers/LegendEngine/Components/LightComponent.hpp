#pragma once

#include <LegendEngine/Components/Component.hpp>
#include <LegendEngine/Math/Types.hpp>

namespace LegendEngine::Components
{
    class LightComponent : public Component
    {
    public:
        enum class LightType
        {
            POINT = 0,
            SPOT = 1,
            SUN = 2,
        };

        void SetIntensity(float intensity);
        void SetColor(const Color& color);
        void SetLightType(LightType lightType);
        float GetIntensity() const;
        Color GetColor() const;
        LightType GetLightType() const;
    private:
        Color m_Color = Color(1.0f);
        LightType m_LightType = LightType::POINT;
    };
}
