#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/ShaderNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;

bool ShaderNative::OnCreate(Resources::ShaderStage* pStages, uint64_t stageCount)
{
    // TODO
    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(),"Vulkan::Shader", 
        "Vulkan shaders are currently unsupported.", 
        LogType::WARN
    );

    LEGENDENGINE_OBJECT_LOG(
        pVulkanRenderer->GetApplication(),"Vulkan::Shader", 
        "Initialized shader.", 
        LogType::DEBUG
    );

    return true;
}

void ShaderNative::OnDispose()
{
    pVulkanRenderer->device.WaitIdle();
}

#endif // VULKAN_API