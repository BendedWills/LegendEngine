#pragma once

#include <typeindex>
#include <LegendEngine/Components/Component.hpp>
#include <LegendEngine/Events/Event.hpp>

namespace LegendEngine::Events
{
    class ComponentRemovedEvent : public Event
    {
    public:
        ComponentRemovedEvent(Objects::Object& object,
            Components::Component& component,
            const std::type_index& typeIndex);

        Objects::Object& GetObject() const;
        Components::Component& GetComponent() const;
        std::type_index GetComponentTypeIndex() const;
    private:
        Objects::Object& m_Object;
        Components::Component& m_Component;
        std::type_index m_ComponentTypeIndex;
    };
}
