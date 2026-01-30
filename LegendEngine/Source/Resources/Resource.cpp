#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Logger.hpp>
#include <LegendEngine/Resources/Resource.hpp>

namespace LegendEngine::Resources
{
    Resource::Resource()
    {
#ifdef LEGENDENGINE_DEBUG
        if (!Application::HasConstructed())
            return;

        LGENG_DEBUG_LOGMANY(
            "Resource created (this = ",
            std::hex, std::showbase,
            reinterpret_cast<size_t>(this), ")");
#endif
    }

    Resource::~Resource()
    {
#ifdef LEGENDENGINE_DEBUG
        if (!Application::HasConstructed())
            return;

        LGENG_DEBUG_LOGMANY(
            "Resource destroyed (this = ",
            std::hex, std::showbase,
            reinterpret_cast<size_t>(this), ")");
#endif
    }
}