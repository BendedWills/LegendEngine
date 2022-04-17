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

        CameraUniforms ubo;
    };
}

#endif //_LEGENDENGINE_CAMERA_HPP