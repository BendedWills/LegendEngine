#ifndef _LEGENDENGINE_CAMERA_HPP
#define _LEGENDENGINE_CAMERA_HPP

#include <LegendEngine/Objects/Object.hpp>

namespace LegendEngine::Objects
{
    class Camera : public Object
    {
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(Camera);
        LEGENDENGINE_NO_COPY(Camera);

		struct CameraUniforms
		{
            Matrix4x4f view;
            Matrix4x4f projection;
		};
        
        Camera();

		// This function is automatically called on window resize
		void SetAspectRatio(float aspect);

        void SetFov(float fov);
        void SetNearZ(float nearZ);
        void SetFarZ(float farZ);

        Vector3f GetForwardVector();
        Vector3f GetForwardVectorPitch();
        Vector3f GetRightVector();

        Matrix4x4f& GetViewMatrix();
		Matrix4x4f& GetProjectionMatrix();
        CameraUniforms* GetUniforms();

        const Vector3f UP = Vector3f(0, 1, 0);
    private:
        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

		void OnPositionChange();
		void OnRotationChange();
        void OnScaleChange();

        float fov = 90.0f;
        float aspect = 1920.0f / 1080.0f;
        float nearZ = 0.1f;
        float farZ = 1000.0f;

        CameraUniforms ubo;
        Vector3f forwardVector;
        Vector3f forwardVectorPitch;
        Vector3f rightVector;
    };
}

#endif //_LEGENDENGINE_CAMERA_HPP