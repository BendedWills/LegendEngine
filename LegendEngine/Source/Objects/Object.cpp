#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>
#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;

Object::Object()
	:
	scale(1.0f),
	Components::ComponentHolder(this)
{
    CalculateTransformMatrix();
	initialized = true;
}

void Object::AddPosition(Vector3f position)
{
    this->position += position;
    CalculateTransformMatrix();
}

void Object::AddRotation(Vector3f rotation)
{
    this->rotation += rotation;
    CalculateTransformMatrix();
}

void Object::AddScale(Vector3f scale)
{
    this->scale += scale;
    CalculateTransformMatrix();
}

void Object::SetPosition(Vector3f position)
{
    this->position = position;
    CalculateTransformMatrix();
}

void Object::SetRotation(Vector3f rotation)
{
    this->rotation = rotation;
    CalculateTransformMatrix();
}

void Object::SetScale(Vector3f scale)
{
    this->scale = scale;
    CalculateTransformMatrix();
}

Vector3f Object::GetPosition()
{
    return position;    
}

Vector3f Object::GetRotation()
{
    return rotation;    
}

Vector3f Object::GetScale()
{
    return scale;
}

Matrix4x4f& Object::GetTransformationMatrix()
{
    return transform;
}

Application* Object::GetApplication()
{
    return pApplication;
}

void Object::CalculateTransformMatrix()
{
    transform = Matrix4x4f::MakeIdentity();
    transform = Matrix4x4f::Translate(transform, position);
    transform = Matrix4x4f::Scale(transform, scale);
	transform = Matrix4x4f::Rotate(transform, Math::Radians(rotation.x),
		Vector3f(1, 0, 0));
	transform = Matrix4x4f::Rotate(transform, Math::Radians(rotation.y),
		Vector3f(0, 1, 0));
	transform = Matrix4x4f::Rotate(transform, Math::Radians(rotation.z),
		Vector3f(0, 0, 1));
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

void Object::OnDispose()
{
	if (nativeSet)
		native->OnDispose();

    ClearComponents();
    
    Tether::VectorUtils::EraseAll(pApplication->objects, this);
}
