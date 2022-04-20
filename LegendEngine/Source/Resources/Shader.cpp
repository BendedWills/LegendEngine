#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Resources;

bool Shader::Init(ShaderStage* pStages, uint64_t stageCount)
{
    if (initialized || !pStages || stageCount == 0)
        return false;
    OnInit();
    
    if (nativeSet)
        if (!native->OnCreate(pStages, stageCount))
            return false;
    
    initialized = true;
    return true;
}

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