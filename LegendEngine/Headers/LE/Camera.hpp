import le.math;

namespace le
{
    class Camera final
    {
    public:
        struct CameraUniforms
        {
            Matrix4x4f view;
            Matrix4x4f projection;
        };

        Camera();

        void SetAspectRatio(float aspect);

        void SetFov(float fov);
        void SetNearZ(float nearZ);
        void SetFarZ(float farZ);

        Vector3f GetForwardVector();
        Vector3f GetRightVector();

        [[nodiscard]] Matrix4x4f GetViewMatrix() const;
        [[nodiscard]] Matrix4x4f GetProjectionMatrix() const;

        [[nodiscard]] bool IsCameraDirty() const;

        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

        const Vector3f UP = Vector3f(0, 1, 0);
    private:
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