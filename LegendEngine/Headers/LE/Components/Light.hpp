#pragma once

#include <LE/Components/Component.hpp>
#include <LE/Math/Math.hpp>

namespace le
{
    class Light : public Component
    {
    public:
        enum class LightType
        {
            POINT = 0,
            SPOT = 1,
            SUN = 2,
        };

        Color m_Color = Color(1.0f);
        LightType m_LightType = LightType::POINT;
    };
}
