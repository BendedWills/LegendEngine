#include <LegendEngine/Graphics/Vulkan/Shader.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool Shader::OnShaderCreate(ShaderStage* pStages, uint64_t stageCount,
    UniformDesc* pDescs, uint64_t uniformCount)
{
    
    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(),"Vulkan::Shader", 
        "Initialized shader", 
        LogType::DEBUG
    );

    return true;
}

void Shader::OnShaderDispose()
{
    
}
