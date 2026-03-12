#include <LegendEngine/Application.hpp>
#include <LegendEngine/IO/Logger.hpp>
#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace LegendEngine::Events
{
    ObjectDestroyedEvent::ObjectDestroyedEvent(Objects::Object& object)
        :
        Event(false),
        m_Object(object)
    {
        LE_DEBUG("Destroyed object (&object = {:#x})",
            reinterpret_cast<size_t>(&object));
    }

    Objects::Object& ObjectDestroyedEvent::GetObject() const
    {
        return m_Object;
    }
}