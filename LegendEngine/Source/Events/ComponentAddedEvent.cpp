#include <LegendEngine/Events/ComponentAddedEvent.hpp>

namespace LegendEngine::Events
{
    ComponentAddedEvent::ComponentAddedEvent(Objects::Object& object,
        Components::Component& component,
        const std::type_index& typeIndex)
        :
        Event(false),
        m_Object(object),
        m_Component(component),
        m_ComponentTypeIndex(typeIndex)
    {}

    Objects::Object& ComponentAddedEvent::GetObject() const
    {
        return m_Object;
    }

    Components::Component& ComponentAddedEvent::GetComponent() const
    {
        return m_Component;
    }

    std::type_index ComponentAddedEvent::GetComponentTypeIndex() const
    {
        return m_ComponentTypeIndex;
    }
}