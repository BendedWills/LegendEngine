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
}

#endif //_LEGENDENGINE_TYPES_HPP