#pragma once

#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Events/Event.hpp>

namespace le
{
    class ObjectDestroyedEvent : public Event
    {
    public:
        explicit ObjectDestroyedEvent(Object& object);

        Object& GetObject() const;
    private:
        Object& m_Object;
    };
}
