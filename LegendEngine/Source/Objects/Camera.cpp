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
	ubo.view = Matrix4x4f::LookAt(position, Vector3f(0, 0, 0), UP);
}

void Camera::CalculateProjectionMatrix()
{
	ubo.projection = Matrix4x4f::PerspectiveRH_ZO(Math::Radians(90.0f),
		1280.0f / 720.0f, 0.01f, 1000.0f);
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
