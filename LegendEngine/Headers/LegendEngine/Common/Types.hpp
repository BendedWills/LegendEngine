#ifndef _LEGENDENGINE_TYPES_HPP
#define _LEGENDENGINE_TYPES_HPP

#include <LegendEngine/Common/WinGDISucks.hpp>

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

    enum class VertexBufferType
    {
        VULKAN,
    };

    enum class ApplicationType
    {
        THREEDIM,
        TWODIM,
    };
}

#endif //_LEGENDENGINE_TYPES_HPP