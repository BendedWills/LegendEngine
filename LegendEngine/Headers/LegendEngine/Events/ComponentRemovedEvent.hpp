#pragma once

#include <typeindex>
#include <LegendEngine/Components/Component.hpp>
#include <LegendEngine/Events/Event.hpp>

namespace le
{
    class ComponentRemovedEvent : public Event
    {
    public:
        ComponentRemovedEvent(Object& object,
            Component& component,
            const std::type_index& typeIndex);

        Object& GetObject() const;
        Component& GetComponent() const;
        std::type_index GetComponentTypeIndex() const;
    private:
        Object& m_Object;
        Component& m_Component;
        std::type_index m_ComponentTypeIndex;
    };
}
