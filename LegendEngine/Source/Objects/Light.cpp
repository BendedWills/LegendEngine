#include <LegendEngine/Objects/Light.hpp>

namespace LegendEngine::Objects
{
    Light::Light()
    {
        AddComponent<Components::LightComponent>();
    }

    Components::LightComponent& Light::GetLightComponent()
    {
        return *GetComponent<Components::LightComponent>();
    }
}
