#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;

Camera::Camera()
{
	// Disable parent object class from calculating the transform matrix since this
	// object is a camera and has its own matrices.
	objCalculateMatrices = false;

	CalculateViewMatrix();
	CalculateProjectionMatrix();
}

void Camera::SetAspectRatio(float aspect)
{
	this->aspect = aspect;
	CalculateProjectionMatrix();
}

void Camera::SetFov(float fov)
{
	this->fov = fov;
	CalculateProjectionMatrix();
}

void Camera::SetNearZ(float nearZ)
{
	this->nearZ = nearZ;
	CalculateProjectionMatrix();
}

void Camera::SetFarZ(float farZ)
{
	this->farZ = farZ;
	CalculateProjectionMatrix();
}

Vector3f Camera::GetForwardVector()
{
	return forwardVector;
}

Vector3f Camera::GetRightVector()
{
	return rightVector;
}

Matrix4x4f& Camera::GetViewMatrix()
{
	return ubo.view;
}

Matrix4x4f& Camera::GetProjectionMatrix()
{
	return ubo.projection;
}

Camera::CameraUniforms* Camera::GetUniforms()
{
	return &ubo;
}

void Camera::CalculateViewMatrix()
{
	Quaternion q = GetRotation();
	
	forwardVector = Math::Rotate(Math::Inverse(q), Vector3f(0, 0, -1.0f));
	rightVector = Math::Rotate(Math::Inverse(q), Vector3f(1.0f, 0, 0));

	Matrix4x4f rot = Matrix4x4f(q);
	Matrix4x4f pos = Math::Translate(Matrix4x4f::MakeIdentity(), -position);
	ubo.view = rot * pos;
}

void Camera::CalculateProjectionMatrix()
{
	ubo.projection = Math::PerspectiveRH_ZO(Math::Radians(fov),
		aspect, nearZ, farZ);
	ubo.projection[1][1] *= -1;
}

void Camera::OnPositionChange()
{
	CalculateViewMatrix();
}

void Camera::OnRotationChange()
{
	CalculateViewMatrix();
}

void Camera::OnScaleChange()
{
	CalculateViewMatrix();
}
