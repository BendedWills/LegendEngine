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
	private:
        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

		void OnPositionChange();
		void OnRotationChange();
        void OnScaleChange();

        CameraUniforms ubo;

		const Vector3f ORIGIN = Vector3f(0, 0, 0);
		const Vector3f UP = Vector3f(0, 1, 0);
		const Vector3f VIEW_DIR = Vector3f(0, 0, -1);
    };
}

#endif //_LEGENDENGINE_CAMERA_HPP