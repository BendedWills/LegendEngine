#include <LegendEngine/Objects/Light.hpp>

namespace le
{
    Light::Light()
    {
        AddComponent<LightComponent>();
    }

    LightComponent& Light::GetLightComponent()
    {
        return *GetComponent<LightComponent>();
    }
}
