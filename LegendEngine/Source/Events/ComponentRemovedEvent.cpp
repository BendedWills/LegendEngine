#include <LegendEngine/Application.hpp>
#include <LegendEngine/IO/Logger.hpp>
#include <LegendEngine/Events/ComponentRemovedEvent.hpp>

namespace LegendEngine::Events
{
    ComponentRemovedEvent::ComponentRemovedEvent(Objects::Object& object,
        Components::Component& component,
        const std::type_index& typeIndex)
        :
        Event(false),
        m_Object(object),
        m_Component(component),
        m_ComponentTypeIndex(typeIndex)
    {
        LGENG_DEBUG("Component removed from Object (&object = {:#x})",
            reinterpret_cast<size_t>(&object));
    }

    Objects::Object& ComponentRemovedEvent::GetObject() const
    {
        return m_Object;
    }

    Components::Component& ComponentRemovedEvent::GetComponent() const
    {
        return m_Component;
    }

    std::type_index ComponentRemovedEvent::GetComponentTypeIndex() const
    {
        return m_ComponentTypeIndex;
    }
}