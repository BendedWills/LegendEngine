#include <LegendEngine/Objects/Scripts/ScriptHolder.hpp>
#include <LegendEngine/Objects/Object.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;

Application* Scripts::_GetObjApp(Object* pObject)
{
    return pObject->GetApplication();
}