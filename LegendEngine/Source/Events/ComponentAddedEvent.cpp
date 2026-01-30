#include <LegendEngine/Events/ComponentAddedEvent.hpp>
#include <LegendEngine/Application.hpp>

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
    {
        LGENG_DEBUG_LOGMANY("Component added to Object (&object = ",
            std::hex, std::showbase,
            reinterpret_cast<size_t>(&object), ")");
    }

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