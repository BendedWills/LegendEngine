#include <LegendEngine/Graphics/Shader.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine;

bool Shader::Init(ShaderStage* pStages, uint64_t stageCount, 
    UniformDesc* pDescs, uint64_t uniformCount)
{
    if (initialized || !pStages || (!pDescs && uniformCount > 0) || stageCount == 0)
        return false;
    
    pRenderer->shaders.push_back(this);
    if (!OnShaderCreate(pStages, stageCount, pDescs, uniformCount))
        return false;
    
    initialized = true;
    return true;
}

RealRenderingAPI Shader::GetType()
{
    return type;
}

void Shader::OnDispose()
{
    OnShaderDispose();
    Tether::VectorUtils::EraseAll(pRenderer->shaders, this);

    LEGENDENGINE_OBJECT_LOG(
        pRenderer->GetApplication(), "Shader", 
        "Disposed shader", 
        LogType::DEBUG
    );
}