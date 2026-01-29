#pragma once

#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Events/Event.hpp>

namespace LegendEngine::Events
{
    class ObjectDestroyedEvent : public Event
    {
    public:
        explicit ObjectDestroyedEvent(Objects::Object& object);

        Objects::Object& GetObject() const;
    private:
        Objects::Object& m_Object;
    };
}
