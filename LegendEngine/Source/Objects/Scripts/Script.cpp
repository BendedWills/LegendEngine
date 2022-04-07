#include <LegendEngine/Objects/Scripts/Script.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Application.hpp>

using namespace LegendEngine::Objects;
using namespace LegendEngine::Objects::Scripts;

void Script::SetRecieveUpdates(bool enabled)
{
    if (updates == enabled)
        return;

    pObject->GetApplication()->SetScriptRecieveUpdates(enabled, this);
}

void Script::SetRecieveRenderUpdates(bool enabled)
{
    if (renderUpdates == enabled)
        return;

    pObject->GetApplication()->SetScriptRecieveRenderUpdates(enabled, this);
}

Object* Script::GetObject()
{
    return pObject;
}
