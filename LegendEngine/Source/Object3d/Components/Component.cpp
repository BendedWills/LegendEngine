#include <LegendEngine/Object3d/Components/Component.hpp>
#include <LegendEngine/Object3d/Object.hpp>

using namespace LegendEngine::Object3d;
using namespace LegendEngine::Object3d::Components;

Component::Component()
{}

Object* Component::GetObject()
{
    return pObject;
}

void Component::SetObject(Object* pObject)
{
    this->pObject = pObject;
}