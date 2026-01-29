#include <LegendEngine/Events/ObjectDestroyedEvent.hpp>

namespace LegendEngine::Events
{
    ObjectDestroyedEvent::ObjectDestroyedEvent(Objects::Object& object)
        :
        Event(false),
        m_Object(object)
    {}

    Objects::Object& ObjectDestroyedEvent::GetObject() const
    {
        return m_Object;
    }
}