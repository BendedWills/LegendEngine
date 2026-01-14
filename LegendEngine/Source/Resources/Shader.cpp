#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Resources;

void Shader::OnResourceDispose()
{
    if (nativeSet)
        native->OnDispose();

    LEGENDENGINE_OBJECT_LOG(
        pApplication, "Shader", 
        "Disposed shader", 
        LogType::DEBUG
    );
}