#include <LegendEngine/Object3d/Object.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Object3d;

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

Application3D* Object::GetApplication()
{
    return pApplication;
}

void Object::CalculateTransformMatrix()
{
    // TODO
}
