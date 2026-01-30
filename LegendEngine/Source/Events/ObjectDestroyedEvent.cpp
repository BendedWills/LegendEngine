#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Logger.hpp>
#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace LegendEngine::Events
{
    ObjectDestroyedEvent::ObjectDestroyedEvent(Objects::Object& object)
        :
        Event(false),
        m_Object(object)
    {
        LGENG_DEBUG_LOGMANY(
            "Destroyed object (&object = ",
            std::hex, std::showbase,
            reinterpret_cast<size_t>(&object), ")");
    }

    Objects::Object& ObjectDestroyedEvent::GetObject() const
    {
        return m_Object;
    }
}