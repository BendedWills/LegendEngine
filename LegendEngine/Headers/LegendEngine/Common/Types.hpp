#ifndef _LEGENDENGINE_TYPES_HPP
#define _LEGENDENGINE_TYPES_HPP

namespace LegendEngine
{
    enum class RenderMode
    {
        UNLIT,
        LIT
    };
    
    enum class LogType
    {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    enum class RenderingAPI
    {
        NONE,
        AUTO_SELECT,
        VULKAN
    };

    enum class RealRenderingAPI
    {
        VULKAN,
    };

    enum class ShaderType
    {
        VERTEX,
        GEOM, // ! CURRENTLY UNSUPPORTED
        COMPUTE, // ! CURRENTLY UNSUPPORTED
        FRAG
    };

    enum class ApplicationType
    {
        THREEDIM,
        TWODIM,
    };
}

#endif //_LEGENDENGINE_TYPES_HPP