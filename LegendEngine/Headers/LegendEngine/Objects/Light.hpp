#pragma once
#include <LegendEngine/Components/LightComponent.hpp>
#include <LegendEngine/Objects/Object.hpp>

namespace LegendEngine::Objects
{
    class Light final : public Object
    {
    public:
        Light();

        Components::LightComponent& GetLightComponent();
    };
}
