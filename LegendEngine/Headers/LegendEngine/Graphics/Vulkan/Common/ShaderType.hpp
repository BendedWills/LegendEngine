#ifndef _LEGENDENGINE_VULKAN_SHADERTYPE_HPP
#define _LEGENDENGINE_VULKAN_SHADERTYPE_HPP
#ifdef VULKAN_API

namespace LegendEngine::Vulkan
{
    enum class ShaderType
    {
        VERTEX,
        GEOM, // ! CURRENTLY UNSUPPORTED
        COMPUTE, // ! CURRENTLY UNSUPPORTED
        FRAG
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SHADERTYPE_HPP