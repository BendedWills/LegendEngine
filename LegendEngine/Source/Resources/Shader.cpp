#include <LegendEngine/Resources/Shader.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Resources;

bool Shader::Init(ShaderStage* pStages, uint64_t stageCount)
{
    if (initialized || !pStages || stageCount == 0)
        return false;
    
    pApplication->shaders.push_back(this);
    if (nativeSet)
        if (!native->OnCreate(pStages, stageCount))
            return false;
    
    initialized = true;
    return true;
}

RenderingAPI Shader::GetType()
{
    return type;
}

void Shader::OnResourceDispose()
{
    if (nativeSet)
        native->OnDispose();

    Tether::VectorUtils::EraseAll(pApplication->shaders, this);

    LEGENDENGINE_OBJECT_LOG(
        pRenderer->GetApplication(), "Shader", 
        "Disposed shader", 
        LogType::DEBUG
    );
}