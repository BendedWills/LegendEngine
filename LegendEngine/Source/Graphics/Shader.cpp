#include <LegendEngine/Graphics/Shader.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;

bool Shader::Init(ShaderStage* pStages, uint64_t stageCount)
{
    if (initialized || !pStages || stageCount == 0)
        return false;
    
    pRenderer->shaders.push_back(this);
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

void Shader::OnDispose()
{
    if (nativeSet)
        native->OnDispose();

    Tether::VectorUtils::EraseAll(pRenderer->shaders, this);

    LEGENDENGINE_OBJECT_LOG(
        pRenderer->GetApplication(), "Shader", 
        "Disposed shader", 
        LogType::DEBUG
    );
}