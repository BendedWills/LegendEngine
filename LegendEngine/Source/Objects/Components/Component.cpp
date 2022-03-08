#include <LegendEngine/Objects/Components/Component.hpp>
#include <LegendEngine/Objects/Object.hpp>

using namespace LegendEngine::Objects;
using namespace LegendEngine::Objects::Components;

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