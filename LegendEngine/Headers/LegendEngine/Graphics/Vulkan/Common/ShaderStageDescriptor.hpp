#ifndef _LEGENDENGINE_VULKAN_SHADERSTAGEDESCRIPTOR_HPP
#define _LEGENDENGINE_VULKAN_SHADERSTAGEDESCRIPTOR_HPP
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
    struct ShaderStageDescriptor
    {
        VkShaderStageFlagBits stage;
        ShaderModule* pModule;
        const char* entrypoint;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SHADERSTAGEDESCRIPTOR_HPP