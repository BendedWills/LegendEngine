#ifndef _LEGENDENGINE_TYPES_HPP
#define _LEGENDENGINE_TYPES_HPP

namespace LegendEngine
{
    enum class LogType
    {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    enum class RenderingAPI
    {
        VULKAN
    };

    enum class ShaderType
    {
        VERTEX,
        GEOM, // ! CURRENTLY UNSUPPORTED
        COMPUTE, // ! CURRENTLY UNSUPPORTED
        FRAG
    };
}

#endif //_LEGENDENGINE_TYPES_HPP