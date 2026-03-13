#include <LegendEngine/Events/ComponentAddedEvent.hpp>
#include <LegendEngine/Application.hpp>

namespace le
{
    ComponentAddedEvent::ComponentAddedEvent(Object& object,
        Component& component,
        const std::type_index& typeIndex)
        :
        Event(false),
        m_Object(object),
        m_Component(component),
        m_ComponentTypeIndex(typeIndex)
    {
        LE_DEBUG("Component added to Object (&object = {:#x})",
            reinterpret_cast<size_t>(&object));
    }

    Object& ComponentAddedEvent::GetObject() const
    {
        return m_Object;
    }

    Component& ComponentAddedEvent::GetComponent() const
    {
        return m_Component;
    }

    std::type_index ComponentAddedEvent::GetComponentTypeIndex() const
    {
        return m_ComponentTypeIndex;
    }
}