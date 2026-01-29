#pragma once

#include <typeindex>
#include <LegendEngine/Components/Component.hpp>
#include <LegendEngine/Events/Event.hpp>
#include <LegendEngine/Objects/Object.hpp>

namespace LegendEngine::Events
{
    class ComponentAddedEvent : public Event
    {
    public:
        ComponentAddedEvent(Objects::Object& object,
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
