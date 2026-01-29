#pragma once

#include <LegendEngine/Objects/Object.hpp>

namespace LegendEngine::Objects
{
    class Camera final : public Object
    {
    public:
        Camera();

        void SetAspectRatio(float aspect);

        void SetFov(float fov);
        void SetNearZ(float nearZ);
        void SetFarZ(float farZ);

        Vector3f GetForwardVector();
        Vector3f GetRightVector();

        Matrix4x4f GetViewMatrix() const;
        Matrix4x4f GetProjectionMatrix() const;

        bool IsCameraDirty() const;

        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

        const Vector3f UP = Vector3f(0, 1, 0);
    private:
        void TransformChanged() override;

        float m_Fov = 90.0f;
        float m_Aspect = 16.0f / 9.0f;
        float m_NearZ = 0.1f;
        float m_FarZ = 1000.0f;

        Vector3f m_ForwardVector;
        Vector3f m_RightVector;

        Matrix4x4f m_View;
        Matrix4x4f m_Projection;

        bool m_CameraDirty = false;
    };
}