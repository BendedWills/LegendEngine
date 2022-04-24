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

Vector3f Camera::GetForwardVectorPitch()
{
	return forwardVectorPitch;
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
	Vector3f rotation = GetRotation();
	float yaw = rotation.x;
	float pitch = rotation.y;
	
	forwardVector.x = cos(Math::Radians(yaw));
	forwardVector.z = sin(Math::Radians(yaw));

	forwardVectorPitch.x = cos(Math::Radians(yaw)) * cos(Math::Radians(pitch));
	forwardVectorPitch.y = sin(Math::Radians(pitch));
	forwardVectorPitch.z = sin(Math::Radians(yaw)) * cos(Math::Radians(pitch));
	
	rightVector.x = cos(Math::Radians(yaw + 90.0f));
	rightVector.z = sin(Math::Radians(yaw + 90.0f));

	ubo.view = Matrix4x4f::LookAt(position, position + forwardVectorPitch, UP);
}

void Camera::CalculateProjectionMatrix()
{
	ubo.projection = Matrix4x4f::PerspectiveRH_ZO(Math::Radians(fov),
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
