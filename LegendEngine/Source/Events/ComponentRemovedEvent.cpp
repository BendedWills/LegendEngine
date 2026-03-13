#include <LegendEngine/Application.hpp>
#include <LegendEngine/IO/Logger.hpp>
#include <LegendEngine/Events/ComponentRemovedEvent.hpp>

namespace le
{
    ComponentRemovedEvent::ComponentRemovedEvent(Object& object,
        Component& component,
        const std::type_index& typeIndex)
        :
        Event(false),
        m_Object(object),
        m_Component(component),
        m_ComponentTypeIndex(typeIndex)
    {
        LE_DEBUG("Component removed from Object (&object = {:#x})",
            reinterpret_cast<size_t>(&object));
    }

    Object& ComponentRemovedEvent::GetObject() const
    {
        return m_Object;
    }

    Component& ComponentRemovedEvent::GetComponent() const
    {
        return m_Component;
    }

    std::type_index ComponentRemovedEvent::GetComponentTypeIndex() const
    {
        return m_ComponentTypeIndex;
    }
}