#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;

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

void Camera::CalculateMatrices()
{

}

void Camera::OnPositionChange()
{
	CalculateMatrices();
}

void Camera::OnRotationChange()
{
	CalculateMatrices();
}

void Camera::OnScaleChange()
{
	CalculateMatrices();
}
