#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>
#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;

void Object::AddPosition(Vector3f position)
{
    this->position.x += position.x;
    this->position.y += position.y;
    this->position.z += position.z;
}

void Object::AddRotation(Vector3f rotation)
{
    this->rotation.x += rotation.x;
    this->rotation.y += rotation.y;
    this->rotation.z += rotation.z;
}

void Object::AddScale(Vector3f scale)
{
    this->scale.x += scale.x;
    this->scale.y += scale.y;
    this->scale.z += scale.z;
}

void Object::SetPosition(Vector3f position)
{
    this->position = position;
}

void Object::SetRotation(Vector3f rotation)
{
    this->rotation = rotation;
}

void Object::SetScale(Vector3f scale)
{
    this->scale = scale;
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

Application* Object::GetApplication()
{
    return pApplication;
}

void Object::CalculateTransformMatrix()
{
    // TODO
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

void Object::SetNative(Ref<IObjectNative> native)
{
	if (nativeSet)
	{
		LEGENDENGINE_OBJECT_LOG(pApplication, "Objects::Object",
			"Tried to set object native twice. Ignoring.", LogType::WARN);
		return;
	}

    this->native = native;
    nativeSet = true;
}

IObjectNative* Object::GetNative()
{
    return native.get();
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
