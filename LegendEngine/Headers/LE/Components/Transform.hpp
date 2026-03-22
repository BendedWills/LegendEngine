#pragma once

#include <LE/Math/Math.hpp>

namespace le
{
    struct Transform
    {
        Vector3f m_position;
        Vector3f m_scale = Vector3f(1.0f);
        Quaternion m_rotation;

        Matrix4x4f m_transform;
    };
}
