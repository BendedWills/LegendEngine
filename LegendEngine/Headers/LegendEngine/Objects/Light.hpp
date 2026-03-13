#pragma once
#include <LegendEngine/Components/LightComponent.hpp>
#include <LegendEngine/Objects/Object.hpp>

namespace le
{
    class Light final : public Object
    {
    public:
        Light();

        LightComponent& GetLightComponent();
    };
}
