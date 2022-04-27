#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>
#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;

void IObjectNative::UpdateUniforms()
{
    if (!pObject->updateUniforms)
        return;

    OnUniformsUpdate();

    pObject->updateUniforms = false;
}

Object::Object()
	:
	scale(1.0f),
	Components::ComponentHolder(this),
    Scripts::ScriptHolder(this)
{
    transform = Matrix4x4f::MakeIdentity();

    CalculateTransformMatrix();
	initialized = true;
    singleDispose = true;
}

void Object::AddPosition(Vector3f position)
{
    this->position += position;
    CalculateTransformMatrix();
    OnPositionChange();
}

void Object::AddScale(Vector3f scale)
{
    this->scale += scale;
    CalculateTransformMatrix();
    OnScaleChange();
}

void Object::SetPosition(Vector3f position)
{
	this->position = position;
	CalculateTransformMatrix();
	OnPositionChange();
}

void Object::SetScale(Vector3f scale)
{
	this->scale = scale;
	CalculateTransformMatrix();
	OnScaleChange();
}

Vector3f Object::GetPosition()
{
	return position;
}

Vector3f Object::GetScale()
{
    return scale;
}

void Object::AddRotation(Vector3f rotation)
{
	this->rotation = Quaternion(this->rotation.GetEulerAngles() + rotation);
	CalculateTransformMatrix();
	OnRotationChange();
}

void Object::SetRotation(Vector3f rotation)
{
	this->rotation = rotation;
	CalculateTransformMatrix();
	OnRotationChange();
}

void Object::SetRotation(Quaternion rotation)
{
	this->rotation = rotation;
	CalculateTransformMatrix();
	OnRotationChange();
}

Vector3f Object::GetEulerRotation()
{
	return rotation.GetEulerAngles();
}

Quaternion Object::GetRotation()
{
	return rotation;
}

void Object::SetEnabled(bool enabled)
{
    if (this->enabled != enabled)
		if (enabled)
			for (Scene* pScene : scenes)
				pScene->OnObjectEnable(this);
		else
			for (Scene* pScene : scenes)
				pScene->OnObjectDisable(this);

    this->enabled = enabled;
}

bool Object::IsEnabled()
{
    return enabled;
}

Matrix4x4f& Object::GetTransformationMatrix()
{
    return transform;
}

void Object::CalculateTransformMatrix()
{
    if (!objCalculateMatrices)
        return;

    transform = Matrix4x4f::MakeIdentity();
    transform = Matrix4x4f::Translate(transform, position);
    transform = Matrix4x4f::Scale(transform, scale);
	transform = Matrix4x4f::Rotate(transform, Math::Radians(rotation.x),
		Vector3f(1, 0, 0));
	transform = Matrix4x4f::Rotate(transform, Math::Radians(rotation.y),
		Vector3f(0, 1, 0));
	transform = Matrix4x4f::Rotate(transform, Math::Radians(rotation.z),
		Vector3f(0, 0, 1));

    updateUniforms = true;
}

void Object::AddToScene(Scene* pScene)
{
    scenes.push_back(pScene);
}

void Object::RemoveFromScene(Scene* pScene)
{
    for (uint64_t i = 0; i < scenes.size(); i++)
        if (scenes[i] == pScene)
            scenes.erase(scenes.begin() + i);
}

void Object::OnDispose()
{
	if (nativeSet)
		native->OnDispose();

	ClearComponents();
	ClearScripts();

	pApplication->_OnObjectDispose(this);
}

void Object::OnComponentAdd(const std::string& typeName, 
    Components::Component* pComponent)
{
    for (Scene* pScene : scenes)
        pScene->OnObjectComponentAdd(this, typeName, pComponent);
}

void Object::OnComponentRemove(const std::string& typeName, 
    Components::Component* pComponent)
{
    for (Scene* pScene : scenes)
        pScene->OnObjectComponentRemove(this, typeName, pComponent);
}
